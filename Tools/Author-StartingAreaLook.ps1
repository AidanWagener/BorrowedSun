<# .SYNOPSIS
Apply the reviewed daylight look to existing main-map actors without regenerating the map.
#>
[CmdletBinding()]
param([switch]$Execute,[string]$ConfigPath)
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath
Assert-Project $ctx
$identity=Assert-EngineLock $ctx
$executable=Join-Path $ctx.Config.EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$arguments=@($ctx.Project,'-run=BSStartingAreaLook','-NullRHI','-unattended','-nop4','-nosplash','-stdout','-FullStdOutLogOutput')
if(-not $Execute){[pscustomobject]@{DryRun=$true;Executable=$executable;Arguments=$arguments}|ConvertTo-Json;return}
$run=New-RunDirectory $ctx 'starting-area-look'
$arguments+='-abslog='+(Join-Path $run 'engine.log')
$start=[DateTime]::UtcNow
$result=@{SchemaVersion=1;Status='Running';Engine=$identity;Git=(Get-GitIdentity $ctx.Root);StartedUtc=$start.ToString('o')}
try {
    $map=Get-Item -LiteralPath (Resolve-ContainedPath $ctx.Root 'Game/Content/BorrowedSun/Maps/L_BorrowedSunDemo.umap')
    Assert-NoReparsePoint $map.FullName
    $map.IsReadOnly=$false
    $null=Invoke-CheckedProcess -FilePath $executable -Arguments $arguments -WorkingDirectory $ctx.Root -TimeoutSeconds 300 -LogPath (Join-Path $run 'process.log')
    $reportPath=Join-Path (Split-Path $ctx.Project -Parent) 'Saved/StartingAreaLook/report.json'
    if((Get-Item -LiteralPath $reportPath).LastWriteTimeUtc -lt $start.AddSeconds(-2)){throw 'Stale starting-area look report.'}
    $report=Get-Content -LiteralPath $reportPath -Raw | ConvertFrom-Json
    if($report.success -ne $true){throw 'Starting-area look authoring did not report success.'}
    Assert-MaterializedAsset (Resolve-ContainedPath $ctx.Root 'Game/Content/BorrowedSun/Maps/L_BorrowedSunDemo.umap')
    Copy-Item -LiteralPath $reportPath -Destination (Join-Path $run 'report.json')
    $result.Status='Passed';Write-Host ('Starting-area look applied. Render review is separate. Evidence: '+$run)
} catch {$result.Status='Failed';throw}
finally {$result.FinishedUtc=[DateTime]::UtcNow.ToString('o');Write-JsonFile (Join-Path $run 'result.json') $result}
