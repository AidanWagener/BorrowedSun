[CmdletBinding()]
param([switch]$Execute,[switch]$Force)
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext
Assert-Project $ctx
$manifestPath=Join-Path $ctx.Root 'SourceArt/InteractionProps/interaction-props-manifest.json'
$manifest=Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$names=@('Cup','Bowl','Lantern','Tool','Cell','Crate','Basket','Bottle','Book','Bundle')
$mapFile=Join-Path $ctx.Root 'Game/Content/BorrowedSun/Maps/L_ObjectLab.umap'
if ((Test-Path -LiteralPath $mapFile) -and -not $Force) { throw 'Object lab map exists; deliberate replacement requires -Force.' }
if ($manifest.schemaVersion -ne 1 -or @($manifest.assets).Count -ne 10) { throw 'Expected the ten authored hand props.' }
for ($i=0;$i -lt 10;$i++) {
    $asset=$manifest.assets[$i]
    $name='SM_Hand'+$names[$i]
    $relative='SourceArt/InteractionProps/'+$name+'.fbx'
    if ($asset.name -cne $name -or $asset.path -cne $relative -or $asset.collisionHulls -lt 1) { throw 'Invalid hand-prop manifest entry.' }
    $actual=(Get-FileHash -LiteralPath (Join-Path $ctx.Root $relative) -Algorithm SHA256).Hash
    if ($actual -ine $asset.sha256) { throw ('Source hash mismatch: '+$name) }
    $destination=Join-Path $ctx.Root ('Game/Content/BorrowedSun/Interaction/Meshes/'+$name+'.uasset')
    if ((Test-Path -LiteralPath $destination) -and -not $Force) { throw ('Owned destination exists; deliberate replacement requires -Force: '+$name) }
}
if (-not $Execute) { [pscustomobject]@{DryRun=$true;VerifiedSources=10;Commandlet='BSInteractionArt';Force=[bool]$Force} | ConvertTo-Json; return }
$identity=Assert-EngineLock $ctx
$run=New-RunDirectory $ctx 'interaction-art'
$report=Join-Path $run 'import-report.json'
$editor=Join-Path $ctx.Config.EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$arguments=@($ctx.Project,'-run=BSInteractionArt',('-SourceRoot='+$ctx.Root),('-Report='+$report),'-unattended','-nop4','-nosplash','-NullRHI')
if ($Force) { $arguments+='-Force' }
$result=@{Status='Running';Engine=$identity;Git=(Get-GitIdentity $ctx.Root);ManifestSha256=(Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash;StartedUtc=[DateTime]::UtcNow.ToString('o')}
try {
    $null=Invoke-CheckedProcess -FilePath $editor -Arguments $arguments -WorkingDirectory $ctx.Root -TimeoutSeconds 600 -LogPath (Join-Path $run 'editor.log')
    $import=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
    if ($import.status -ne 'Passed' -or @($import.assets).Count -ne 10) { throw 'Missing complete ten-prop import report.' }
    if ($import.map -cne '/Game/BorrowedSun/Maps/L_ObjectLab' -or -not (Test-Path -LiteralPath $mapFile -PathType Leaf)) { throw 'Object lab map was not saved.' }
    for ($i=0;$i -lt 10;$i++) {
        if ($import.assets[$i].name -cne $manifest.assets[$i].name -or $import.assets[$i].collisionHulls -ne $manifest.assets[$i].collisionHulls) { throw 'Imported hull identity mismatch.' }
        $file=Join-Path $ctx.Root ('Game/Content/BorrowedSun/Interaction/Meshes/'+$import.assets[$i].name+'.uasset')
        if (-not (Test-Path -LiteralPath $file -PathType Leaf)) { throw 'Import report is missing its package.' }
    }
    $result.Status='Passed'; Write-Host ('Interaction art imported. Evidence: '+$run)
} catch { $result.Status='Failed'; throw }
finally { $result.FinishedUtc=[DateTime]::UtcNow.ToString('o'); Write-JsonFile (Join-Path $run 'result.json') $result }
