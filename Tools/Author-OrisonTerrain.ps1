<# .SYNOPSIS
Import the canonical Orison macro terrain into the existing main game map.
#>
[CmdletBinding()]
param([switch]$Execute,[switch]$ReplaceOwned,[string]$ConfigPath)
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath
Assert-Project $ctx
$identity=Assert-EngineLock $ctx
$source=Resolve-ContainedPath $ctx.Root 'SourceArt/World/Orison/Orison_MacroTerrain_1009.r16'
$metadataPath=Resolve-ContainedPath $ctx.Root 'SourceArt/World/Orison/Orison_MacroTerrain_1009.json'
Assert-NoReparsePoint $source
Assert-NoReparsePoint $metadataPath
$metadata=Get-Content -LiteralPath $metadataPath -Raw | ConvertFrom-Json
if($metadata.schemaVersion -ne 1 -or ($metadata.resolution -join ',') -ne '1009,1009') {throw 'Unsupported regional terrain source.'}
if((Get-Item -LiteralPath $source).Length -ne (1009*1009*2)) {throw 'Terrain R16 byte count is invalid.'}
if(-not $metadata.sha256 -or (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $metadata.sha256) {throw 'Terrain source hash differs from its generated metadata.'}
if(($metadata.runtimeOriginMetres -join ',') -ne '-4754.3,-1365.3,91.92') {throw 'Terrain origin differs from the compiled first-import coordinate contract.'}
$expectedScale=@((2700000.0/1008.0),(2400000.0/1008.0),800.0)
if(@($metadata.engineScaleCm).Count -ne 3){throw 'Terrain source must declare all three scales.'}
for($axis=0;$axis -lt 3;$axis++){
    if([double]::IsNaN([double]$metadata.engineScaleCm[$axis]) -or [math]::Abs([double]$metadata.engineScaleCm[$axis]-$expectedScale[$axis]) -gt .00001){throw 'Terrain source scale differs from the compiled import contract.'}
}
$canon=Resolve-ContainedPath $ctx.Root 'Docs/World/Cities/orison_city_plan.json'
if((Get-FileHash -LiteralPath $canon -Algorithm SHA256).Hash -ine $metadata.source_city_plan_sha256){throw 'Orison city plan changed; regenerate and check the heightfield before importing.'}
$executable=Join-Path $ctx.Config.EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$arguments=@($ctx.Project,'-run=BSOrisonTerrain','-NullRHI','-unattended','-nop4','-nosplash','-stdout','-FullStdOutLogOutput')
if($ReplaceOwned){$arguments+='-ReplaceOwned'}
if(-not $Execute){[pscustomobject]@{DryRun=$true;Source=$source;Executable=$executable;Arguments=$arguments}|ConvertTo-Json;return}
$run=New-RunDirectory $ctx 'orison-terrain'
$arguments+='-abslog='+(Join-Path $run 'engine.log')
$start=[DateTime]::UtcNow
$result=@{SchemaVersion=1;Status='Running';SourceSha256=$metadata.sha256;Engine=$identity;Git=(Get-GitIdentity $ctx.Root);StartedUtc=$start.ToString('o')}
try {
    # Git LFS marks lockable binaries read-only. Touch only the three owned outputs.
    foreach($relative in @('Game/Content/BorrowedSun/Maps/L_BorrowedSunDemo.umap','Game/Content/BorrowedSun/World/Orison/M_OrisonMacroGround.uasset','Game/Content/BorrowedSun/World/Orison/M_OrisonPlanningSea.uasset')){
        $ownedPath=Resolve-ContainedPath $ctx.Root $relative
        Assert-NoReparsePoint $ownedPath
        if(Test-Path -LiteralPath $ownedPath -PathType Leaf){(Get-Item -LiteralPath $ownedPath).IsReadOnly=$false}
    }
    $null=Invoke-CheckedProcess -FilePath $executable -Arguments $arguments -WorkingDirectory $ctx.Root -TimeoutSeconds 900 -LogPath (Join-Path $run 'process.log')
    $reportPath=Join-Path (Split-Path $ctx.Project -Parent) 'Saved/OrisonTerrain/authoring.json'
    if((Get-Item -LiteralPath $reportPath).LastWriteTimeUtc -lt $start.AddSeconds(-2)){throw 'Stale regional terrain report.'}
    $report=Get-Content -LiteralPath $reportPath -Raw | ConvertFrom-Json
    if($report.success -ne $true -or $report.components -ne 256 -or $report.resolution -ne 1009){throw 'Regional terrain import is incomplete.'}
    foreach($relative in @('Game/Content/BorrowedSun/Maps/L_BorrowedSunDemo.umap','Game/Content/BorrowedSun/World/Orison/M_OrisonMacroGround.uasset','Game/Content/BorrowedSun/World/Orison/M_OrisonPlanningSea.uasset')){
        Assert-MaterializedAsset (Resolve-ContainedPath $ctx.Root $relative)
    }
    Copy-Item -LiteralPath $reportPath -Destination (Join-Path $run 'authoring.json')
    $result.Status='Passed'
    Write-Host ('Orison terrain imported into main game. Evidence: '+$run)
} catch {$result.Status='Failed';throw}
finally {$result.FinishedUtc=[DateTime]::UtcNow.ToString('o');Write-JsonFile (Join-Path $run 'result.json') $result}
