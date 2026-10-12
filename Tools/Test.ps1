[CmdletBinding()]
param([Alias('UE_ROOT')][string]$EngineRoot,[string]$ConfigPath,[switch]$Execute,[switch]$NullRHI)
. "$PSScriptRoot/Common.ps1"
$ctx = Get-WorkspaceContext -ConfigPath $ConfigPath -EngineRoot $EngineRoot
Assert-Project $ctx
$cfg=$ctx.Config
$engine=$cfg.EngineRoot; if (-not $engine) { $engine='<SET-ENGINE-ROOT>' }
$editor=Join-Path $engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
if (-not $Execute) {
    [pscustomobject]@{DryRun=$true; Tool=$editor; Filter=$cfg.TestFilter; MinimumTests=$cfg.MinimumTests; ExpectedTests=$cfg.ExpectedTests; NullRHI=[bool]$NullRHI; Note='No tests ran. Execute creates a fresh JSON report and requires every selected test to succeed.'} | ConvertTo-Json -Depth 6
    return
}
if (-not $cfg.EngineRoot -or -not (Test-Path -LiteralPath $editor -PathType Leaf)) { throw 'Configured UnrealEditor-Cmd.exe is missing.' }
$engineIdentity=Assert-EngineLock $ctx
$run=New-RunDirectory $ctx 'tests'
$report=Join-Path $run 'Automation'
$arguments=@($ctx.Project,'-unattended','-nop4','-nosplash','-nosound',('-ExecCmds=Automation RunTest ' + $cfg.TestFilter),('-ReportExportPath=' + $report),'-TestExit=Automation Test Queue Empty','-log')
if ($NullRHI) { $arguments += '-NullRHI' }
$result=@{SchemaVersion=1; Status='Running'; Filter=$cfg.TestFilter; NullRHI=[bool]$NullRHI; Engine=$engineIdentity; Git=(Get-GitIdentity $ctx.Root); StartedUtc=[DateTime]::UtcNow.ToString('o')}
try {
    $null=Invoke-CheckedProcess -FilePath $editor -Arguments $arguments -WorkingDirectory $ctx.Root -TimeoutSeconds ([int]$cfg.TimeoutMinutes * 60) -LogPath (Join-Path $run 'editor.log')
    $validated=Test-AutomationReport -ReportPath (Join-Path $report 'index.json') -MinimumTests $cfg.MinimumTests -ExpectedTests $cfg.ExpectedTests
    $result.Status='Passed'; $result.Count=$validated.Count
    Write-Host ('Automation passed (' + $validated.Count + ' tests). Evidence: ' + $run)
} catch { $result.Status='Failed'; throw }
finally { $result.FinishedUtc=[DateTime]::UtcNow.ToString('o'); Write-JsonFile (Join-Path $run 'test-result.json') $result }
