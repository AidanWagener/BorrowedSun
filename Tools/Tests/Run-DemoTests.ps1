# Report-gate and independent launch regressions. No Unreal, network access or game saves are touched.
[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
Set-StrictMode -Version 2.0
$toolsDirectory=Split-Path $PSScriptRoot -Parent
. (Join-Path $toolsDirectory 'Common.ps1')
$scriptPath=Join-Path $toolsDirectory 'Demo.ps1'
$tokens=$null; $parseErrors=$null
$ast=[Management.Automation.Language.Parser]::ParseFile($scriptPath,[ref]$tokens,[ref]$parseErrors)
if (@($parseErrors).Count -ne 0) { throw ('Demo.ps1 parse failed: ' + ($parseErrors | Out-String)) }
foreach ($functionName in @('Test-DemoRuntimeReport','Start-DemoInteractiveProcess')) {
    $gate=$ast.Find({param($node) $node -is [Management.Automation.Language.FunctionDefinitionAst] -and $node.Name -eq $functionName},$true)
    if (-not $gate) { throw ('Demo helper was not found: ' + $functionName) }
    # Import trusted local helper functions only, without running the script entry point.
    Invoke-Expression $gate.Extent.Text
}
$workspace=Split-Path $toolsDirectory -Parent
$fixtureRoot=Resolve-ContainedPath $workspace ('Artifacts/tooling-tests/demo-' + [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ') + '-' + [Guid]::NewGuid().ToString('N').Substring(0,8))
$null=New-Item -ItemType Directory -Path $fixtureRoot
$reportPath=Join-Path $fixtureRoot 'Result-QA_fixture.json'
$startedUtc=[DateTime]::UtcNow.AddSeconds(-1)
$script:passed=0

function New-ValidDemoReport {
    $names=@('Clinic.IsolatedSaveNamespace','Clinic.RenderedRuntimeRequired','Clinic.RuntimeReady')
    foreach ($route in @('Clinic','Evacuation')) {
        foreach ($step in @('ResetCheckpoint','FreshJourney','WalkingMakesProgress','AcquireThroughInteraction',
            'BoardThroughInteraction','ReachRelayByFlight','AuthorizeSingleLease','ReturnAndDisembark',
            'InstallThroughInteraction','DuplicateInteractionHarmless','ManualCheckpointSaved',
            'CheckpointReadFromDisk','CompletedLeaseSurvivesLoad','RouteWithinBudget')) {
            $names += $route + '.' + $step
        }
    }
    return [pscustomobject]@{schemaVersion=1; namespace='QA_fixture'; success=$true; canEverRender=$true;
        nullRHIRequested=$false; screenshotsRequested=$false;
        checks=@($names | ForEach-Object { [pscustomobject]@{name=$_; passed=$true; elapsedSeconds=1; details='Fixture'} })}
}
function Save-Fixture($Report) { Write-JsonFile -Path $reportPath -Value $Report }
function Invoke-Gate { Test-DemoRuntimeReport -ReportPath $reportPath -ExpectedNamespace 'QA_fixture' -StartedUtc $startedUtc }
function Expect-Rejection([string]$Name,[scriptblock]$Action) {
    $rejected=$false
    try { $null=& $Action } catch { $rejected=$true }
    if (-not $rejected) { throw ('REGRESSION: gate accepted ' + $Name) }
    $script:passed++
    Write-Host ('PASS rejects ' + $Name)
}

Save-Fixture (New-ValidDemoReport)
$valid=Invoke-Gate
if (-not $valid.Passed -or $valid.Count -ne 31 -or $valid.RequiredCount -ne 29) { throw 'Complete two-route report failed its positive control.' }
$script:passed++
Write-Host 'PASS accepts complete rendered two-route report'

Expect-Rejection 'native success without a report' {
    Test-DemoRuntimeReport -ReportPath (Join-Path $fixtureRoot 'missing.json') -ExpectedNamespace 'QA_fixture' -StartedUtc $startedUtc
}
$report=New-ValidDemoReport; $report.namespace='QA_other'; Save-Fixture $report
Expect-Rejection 'another run namespace' { Invoke-Gate }
Save-Fixture (New-ValidDemoReport)
[IO.File]::SetLastWriteTimeUtc($reportPath,[DateTime]::UtcNow.AddDays(-1))
Expect-Rejection 'stale report with matching namespace' { Invoke-Gate }
$report=New-ValidDemoReport; $report.checks=@(); Save-Fixture $report
Expect-Rejection 'zero checks despite success true' { Invoke-Gate }
$report=New-ValidDemoReport; $report.checks=@($report.checks | Where-Object { $_.name -notlike 'Evacuation.*' }); Save-Fixture $report
Expect-Rejection 'Clinic-only completion' { Invoke-Gate }
$report=New-ValidDemoReport; $report.checks=@($report.checks | Where-Object { $_.name -ne 'Evacuation.CheckpointReadFromDisk' }); Save-Fixture $report
Expect-Rejection 'missing persisted checkpoint evidence' { Invoke-Gate }
$report=New-ValidDemoReport; $report.checks=@($report.checks | Where-Object { $_.name -ne 'Clinic.WalkingMakesProgress' }); Save-Fixture $report
Expect-Rejection 'route without movement evidence' { Invoke-Gate }
$report=New-ValidDemoReport; $report.checks += $report.checks[0]; Save-Fixture $report
Expect-Rejection 'duplicated mandatory check' { Invoke-Gate }
$report=New-ValidDemoReport; $report.checks[10].passed=$false; Save-Fixture $report
Expect-Rejection 'failed check despite success true' { Invoke-Gate }
$report=New-ValidDemoReport; $report.success=$false; Save-Fixture $report
Expect-Rejection 'failure summary with passing checks' { Invoke-Gate }
$report=New-ValidDemoReport; $report.success='true'; Save-Fixture $report
Expect-Rejection 'string pretending to be success boolean' { Invoke-Gate }
$report=New-ValidDemoReport; $report.canEverRender=$false; Save-Fixture $report
Expect-Rejection 'non-rendered execution' { Invoke-Gate }
$report=New-ValidDemoReport; $report.nullRHIRequested=$true; Save-Fixture $report
Expect-Rejection 'NullRHI execution' { Invoke-Gate }
$report=New-ValidDemoReport; $report.PSObject.Properties.Remove('canEverRender'); Save-Fixture $report
Expect-Rejection 'missing render capability field' { Invoke-Gate }
[IO.File]::WriteAllText($reportPath,'{"broken":')
Expect-Rejection 'truncated JSON' { Invoke-Gate }
Save-Fixture (New-ValidDemoReport)
Expect-Rejection 'requested screenshots without capture flag' {
    Test-DemoRuntimeReport -ReportPath $reportPath -ExpectedNamespace 'QA_fixture' -StartedUtc $startedUtc -RequireScreenshots
}
$report=New-ValidDemoReport; $report.screenshotsRequested=$true; Save-Fixture $report
Expect-Rejection 'requested screenshots missing from disk' {
    Test-DemoRuntimeReport -ReportPath $reportPath -ExpectedNamespace 'QA_fixture' -StartedUtc $startedUtc -RequireScreenshots
}
Expect-Rejection 'Force outside AuthorMap' { & $scriptPath -Mode Launch -Force }
Expect-Rejection 'packaged validation with unknown report directory' { & $scriptPath -Mode Validate -PackageExecutable 'C:\missing\BorrowedSun.exe' }

$childScript=Join-Path $fixtureRoot 'interactive child.ps1'
$childOutput=Join-Path $fixtureRoot 'interactive arguments.json'
$childSource=@'
param([string]$OutputPath,[string]$Spaced,[string]$Quoted,[string]$Slashed)
Start-Sleep -Seconds 3
[IO.File]::WriteAllText($OutputPath,(@($Spaced,$Quoted,$Slashed) | ConvertTo-Json))
'@
[IO.File]::WriteAllText($childScript,$childSource)
$watch=[Diagnostics.Stopwatch]::StartNew()
$launched=Start-DemoInteractiveProcess -FilePath ((Get-Process -Id $PID).Path) -WorkingDirectory $fixtureRoot -Arguments @('-NoProfile','-NonInteractive','-File',$childScript,$childOutput,'with space','a"quote','path\')
$watch.Stop()
$child=Get-Process -Id $launched.ProcessId -ErrorAction Stop
try {
    if ($watch.ElapsedMilliseconds -ge 2500 -or $launched.Status -ne 'Launched' -or $child.HasExited) { throw 'Interactive launch did not return while its process remained alive.' }
    $script:passed++
    Write-Host 'PASS interactive launch returns promptly with a live PID'
    if (-not $child.WaitForExit(7000)) { throw 'Independent fixture process did not finish within its test budget.' }
} finally { $child.Dispose() }
if (-not (Test-Path -LiteralPath $childOutput -PathType Leaf)) { throw 'Independent process was terminated before writing its output.' }
$received=Get-Content -LiteralPath $childOutput -Raw | ConvertFrom-Json
if ($received.Count -ne 3 -or $received[0] -cne 'with space' -or $received[1] -cne 'a"quote' -or $received[2] -cne 'path\') { throw 'Interactive launch corrupted native arguments.' }
$script:passed++
Write-Host 'PASS independent process survives launcher disposal and receives quoted arguments intact'

Write-JsonFile (Join-Path $fixtureRoot 'demo-tooling-result.json') @{Status='Passed'; Count=$script:passed; UnrealLaunched=$false; FinishedUtc=[DateTime]::UtcNow.ToString('o')}
Write-Host ($script:passed.ToString() + ' demo tooling regressions passed. Fixtures: ' + $fixtureRoot)
