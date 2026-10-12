<#
.SYNOPSIS
Import the reviewed city texture maps and author the original modular mesh kit.
.DESCRIPTION
Dry run by default. Run after compiling the editor and fetching the reviewed source maps.
Force deliberately regenerates only Game/Content/BorrowedSun/City. Keep the editor closed.
#>
[CmdletBinding()]
param([switch]$Execute,[switch]$Force,[string]$ConfigPath)
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath
Assert-Project $ctx
$identity=Assert-EngineLock $ctx
$register=Get-Content -LiteralPath (Join-Path $ctx.Root 'Docs/Registers/city-material-intake.json') -Raw | ConvertFrom-Json
$sourceCount=0
foreach($asset in $register.assets) {
    foreach($map in $asset.maps) {
        $file=Resolve-ContainedPath $ctx.Root $map.path
        Assert-NoReparsePoint $file
        if ((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash -ine $map.sha256) { throw ('Source map hash differs: '+$map.path) }
        $sourceCount++
    }
}
if ($sourceCount -ne 12) { throw 'The reviewed city source set requires exactly 12 maps.' }
$propRegister=Get-Content -LiteralPath (Join-Path $ctx.Root 'Docs/Registers/city-prop-intake.json') -Raw | ConvertFrom-Json
$propCount=0
foreach($asset in $propRegister.assets) {
    foreach($source in $asset.files) {
        $file=Resolve-ContainedPath $ctx.Root $source.path
        Assert-NoReparsePoint $file
        if((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash -ine $source.sha256){throw ('Source prop hash differs: '+$source.path)}
        $propCount++
    }
}
if($propCount -ne 11){throw 'The reviewed prop set requires two FBX meshes and nine texture maps.'}
$executable=Join-Path $ctx.Config.EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$arguments=@($ctx.Project,'-run=BSCityArt','-NullRHI','-unattended','-nop4','-nosplash','-stdout','-FullStdOutLogOutput')
if($Force){$arguments+='-Force'}
if(-not $Execute){[pscustomobject]@{DryRun=$true;SourceMaps=$sourceCount;Force=[bool]$Force;Executable=$executable;Arguments=$arguments}|ConvertTo-Json;return}
$run=New-RunDirectory $ctx 'city-art'
$arguments+='-abslog='+(Join-Path $run 'engine.log')
$result=@{SchemaVersion=1;Status='Running';Engine=$identity;SourceMaps=$sourceCount;Git=(Get-GitIdentity $ctx.Root);StartedUtc=[DateTime]::UtcNow.ToString('o')}
try {
    $null=Invoke-CheckedProcess -FilePath $executable -Arguments $arguments -WorkingDirectory $ctx.Root -TimeoutSeconds 900 -LogPath (Join-Path $run 'process.log')
    $content=Join-Path (Split-Path $ctx.Project -Parent) 'Content/BorrowedSun/City'
    $assets=@(Get-ChildItem -LiteralPath $content -Recurse -File -Filter '*.uasset')
    if($assets.Count -ne 42){throw 'Expected exactly 21 texture, 7 material and 14 mesh packages.'}
    foreach($asset in $assets){Assert-MaterializedAsset $asset.FullName}
    $result.Status='Passed';$result.AuthoredPackages=$assets.Count
    Write-Host ('City art authored and source hashes verified: '+$run)
} catch {$result.Status='Failed';throw}
finally {$result.FinishedUtc=[DateTime]::UtcNow.ToString('o');Write-JsonFile (Join-Path $run 'city-art-result.json') $result}
