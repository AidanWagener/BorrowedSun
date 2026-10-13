<# .SYNOPSIS
Import the reviewed starting-market assets through the pinned Unreal editor.
.PARAMETER SurfacesOnly
Rewrite the nine architectural materials using textures already imported into Unreal.
Requires a successful full import of the matching source revision first. Source hash
checks verify files on disk; they do not prove existing Unreal meshes/textures match.
After changing geometry, source textures or paving period, run the full import first.
#>
[CmdletBinding()]
param([switch]$Execute,[switch]$Force,[switch]$WorkshopOnly,[switch]$SurfacesOnly,[string]$ConfigPath)
if($WorkshopOnly -and $SurfacesOnly){throw 'Choose WorkshopOnly or SurfacesOnly, not both.'}
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath
Assert-Project $ctx
$identity=Assert-EngineLock $ctx
$register=Get-Content -LiteralPath (Join-Path $ctx.Root 'Docs/Registers/market-asset-intake.json') -Raw | ConvertFrom-Json
$sources=@($register.assets | ForEach-Object { $_.files })
$kit=Get-Content -LiteralPath (Join-Path $ctx.Root 'SourceArt/MarketKit/market-kit-manifest.json') -Raw | ConvertFrom-Json
if(-not $WorkshopOnly -and $kit.pavingMaterialPeriodCm -ne 200){throw 'Paving height geometry and compiled material must both use the reviewed 200 cm period.'}
$sources+=@($kit.assets)
$cloth=Get-Content -LiteralPath (Join-Path $ctx.Root 'SourceArt/Generated/IndigoSunWeave.provenance.json') -Raw | ConvertFrom-Json
$sources+=@([pscustomobject]@{path='SourceArt/Generated/IndigoSunWeave.png';sha256=$cloth.sha256})
$workshop=Get-Content -LiteralPath (Join-Path $ctx.Root 'SourceArt/LanternWorkshop/workshop-manifest.json') -Raw | ConvertFrom-Json
if($workshop.materialCount -ne 7){throw 'Workshop manifest must declare the seven authored workshop materials.'}
$sources+=@($workshop.assets)+@($workshop.files)
$wood=Get-Content -LiteralPath (Join-Path $ctx.Root 'Docs/Registers/workshop-surface-intake.json') -Raw | ConvertFrom-Json
if($wood.schemaVersion -ne 1 -or @($wood.files).Count -ne 3){throw 'Workshop wood intake must contain the three reviewed PBR maps.'}
$sources+=@($wood.files)
foreach($source in $sources) {
    $file=Resolve-ContainedPath $ctx.Root $source.path
    Assert-NoReparsePoint $file
    if(-not $source.sha256 -or (Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash -ine $source.sha256){throw ('Source hash missing or differs: '+$source.path)}
}
$executable=Join-Path $ctx.Config.EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$arguments=@($ctx.Project,'-run=BSMarketArt','-NullRHI','-unattended','-nop4','-nosplash','-stdout','-FullStdOutLogOutput')
if($Force){$arguments+='-Force'}
if($WorkshopOnly){$arguments+='-WorkshopOnly'}
if($SurfacesOnly){$arguments+='-SurfacesOnly'}
if(-not $Execute){[pscustomobject]@{DryRun=$true;SourceFiles=$sources.Count;Force=[bool]$Force;Executable=$executable;Arguments=$arguments}|ConvertTo-Json;return}
$run=New-RunDirectory $ctx 'market-art'
$arguments+='-abslog='+(Join-Path $run 'engine.log')
$start=[DateTime]::UtcNow
$result=@{SchemaVersion=1;Status='Running';Engine=$identity;SourceFiles=$sources.Count;Git=(Get-GitIdentity $ctx.Root);StartedUtc=$start.ToString('o')}
try {
    $null=Invoke-CheckedProcess -FilePath $executable -Arguments $arguments -WorkingDirectory $ctx.Root -TimeoutSeconds 900 -LogPath (Join-Path $run 'process.log')
    $reportPath=Join-Path (Split-Path $ctx.Project -Parent) 'Saved/MarketAuthoring/market-assets.json'
    if((Get-Item -LiteralPath $reportPath).LastWriteTimeUtc -lt $start.AddSeconds(-2)){throw 'Stale authoring report.'}
    $report=Get-Content -LiteralPath $reportPath -Raw | ConvertFrom-Json
    $expected=1+9+$kit.assetCount
    foreach($asset in $register.assets){foreach($mat in $asset.materials){$expected+=@($mat.maps.PSObject.Properties).Count;if($asset.type -eq 'model'){$expected++}};if($asset.type -eq 'model'){$expected++}}
    if($WorkshopOnly){$expected=0}
    $expected+=1+$workshop.materialCount+$workshop.assetCount+@($wood.files).Count
    if($SurfacesOnly){$expected=9}
    $paths=@($report.assets | Sort-Object -Unique)
    if($report.success -ne $true -or $report.assetCount -ne $expected -or $paths.Count -ne $expected){throw ('Market report is incomplete. Expected '+$expected+'; found '+$paths.Count)}
    $content=Join-Path (Split-Path $ctx.Project -Parent) 'Content'
    foreach($path in $paths){
        if(-not $path.StartsWith('/Game/BorrowedSun/Market/')){throw 'Authoring report escaped its owned content directory.'}
        $package=$path.Split('.')[0].Substring(6)+'.uasset'
        Assert-MaterializedAsset (Resolve-ContainedPath $content $package)
    }
    Copy-Item -LiteralPath $reportPath -Destination (Join-Path $run 'market-assets.json')
    $result.Status='Passed';$result.AuthoredPackages=$paths.Count;$result.WorkshopOnly=[bool]$WorkshopOnly;$result.SurfacesOnly=[bool]$SurfacesOnly
    Write-Host ('Market art authored and source hashes verified: '+$run)
} catch {$result.Status='Failed';throw}
finally {$result.FinishedUtc=[DateTime]::UtcNow.ToString('o');Write-JsonFile (Join-Path $run 'market-art-result.json') $result}
