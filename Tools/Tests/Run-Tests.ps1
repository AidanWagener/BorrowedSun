[CmdletBinding()]
param()
. "$PSScriptRoot/../Common.ps1"
$toolsRoot=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $toolsRoot -Parent
$context=Get-WorkspaceContext
$run=New-RunDirectory $context 'tooling-tests'
$passed=0
function Assert-True { param([bool]$Value,[string]$Message); if (-not $Value) { throw $Message } }
function Expect-Failure {
    param([scriptblock]$Action,[string]$Message)
    $failed=$false
    try { & $Action | Out-Null } catch { $failed=$true }
    Assert-True $failed $Message
    $script:passed++
}
function Pass { $script:passed++ }

# A real child cmd.exe exercises paths with spaces and batch argument boundaries.
$mock=Join-Path $run 'Mock Engine With Spaces'
$batch=Join-Path $mock 'Engine/Build/BatchFiles'
$null=New-Item -ItemType Directory -Path $batch
$engineLock=Get-Content -LiteralPath (Join-Path $workspace 'Config/engine-lock.json') -Raw | ConvertFrom-Json
$versionParts=$engineLock.engine.version.Split('.')
Write-JsonFile (Join-Path $mock 'Engine/Build/Build.version') @{MajorVersion=[int]$versionParts[0];MinorVersion=[int]$versionParts[1];PatchVersion=[int]$versionParts[2];Changelist=$engineLock.engine.changelist}
$echoBatch=Join-Path $batch 'Echo.cmd'
@'
@echo off
echo first=[%~1]
echo second=[%~2]
exit /b 0
'@ | Set-Content -LiteralPath $echoBatch -Encoding ASCII
$echo=Invoke-CheckedProcess -FilePath $echoBatch -Arguments @('path with spaces','-Project=C:\Project With Spaces\Game.uproject') -WorkingDirectory $workspace
Assert-True ($echo.StdOut.Contains('first=[path with spaces]') -and $echo.StdOut.Contains('second=[-Project=C:\Project With Spaces\Game.uproject]')) 'Batch argument quoting failed.'
Pass
Expect-Failure { Invoke-CheckedProcess -FilePath $echoBatch -Arguments @('unsafe&value') -WorkingDirectory $workspace } 'Shell metacharacter was accepted.'
$failBatch=Join-Path $batch 'Fail.cmd'
"@echo off`r`nexit /b 7" | Set-Content -LiteralPath $failBatch -Encoding ASCII
Expect-Failure { Invoke-CheckedProcess -FilePath $failBatch -WorkingDirectory $workspace } 'Native nonzero exit was ignored.'
$shell=(Get-Process -Id $PID).Path
$timeoutLog=Join-Path $run 'timeout.log'
Expect-Failure { Invoke-CheckedProcess -FilePath $shell -Arguments @('-NoProfile','-Command','Write-Output "before timeout"; Start-Sleep -Seconds 5') -WorkingDirectory $workspace -TimeoutSeconds 1 -LogPath $timeoutLog } 'Child timeout was ignored.'
Assert-True ((Get-Content -LiteralPath $timeoutLog -Raw).Contains('before timeout')) 'Timeout diagnostic output was lost.'; Pass

$report=Join-Path $run 'report.json'
Write-JsonFile $report @{tests=@(@{state='Success';fullTestPath='BorrowedSun.Smoke'}); failed=0;notRun=0;inProcess=0}
$ok=Test-AutomationReport $report 1 @('BorrowedSun.Smoke')
Assert-True ($ok.Count -eq 1) 'Success report was rejected.'; Pass
Expect-Failure { Test-AutomationReport $report 2 } 'Insufficient count accepted.'
Expect-Failure { Test-AutomationReport $report 1 @('BorrowedSun.Missing') } 'Missing expected test accepted.'
foreach ($state in @('Fail','NotRun','InProcess','Skipped','Unknown')) {
    Write-JsonFile $report @{tests=@(@{state=$state;fullTestPath='BorrowedSun.Smoke'})}
    Expect-Failure { Test-AutomationReport $report } ('Accepted test state ' + $state)
}
Write-JsonFile $report @{tests=@()}
Expect-Failure { Test-AutomationReport $report } 'Zero tests accepted.'
Expect-Failure { Test-AutomationReport (Join-Path $run 'missing.json') } 'Missing report accepted.'
Expect-Failure { Resolve-ContainedPath $run '../escape' } 'Traversal was accepted.'

# Full Build.ps1 route against mock Build.bat and RunUAT.bat, including packaging.
$projectDir=Join-Path $run 'Mock Project'
$null=New-Item -ItemType Directory -Path (Join-Path $projectDir 'Content/Maps')
$null=New-Item -ItemType Directory -Path (Join-Path $projectDir 'Config')
@'
[/Script/EngineSettings.GameMapsSettings]
GameDefaultMap=/Game/Maps/Smoke
'@ | Set-Content -LiteralPath (Join-Path $projectDir 'Config/DefaultEngine.ini') -Encoding ASCII
'{"FileVersion":3}' | Set-Content -LiteralPath (Join-Path $projectDir 'BorrowedSun.uproject') -Encoding ASCII
'fake map bytes for tooling test only' | Set-Content -LiteralPath (Join-Path $projectDir 'Content/Maps/Smoke.umap') -Encoding ASCII
$capture=Join-Path $run 'invocations.txt'
$flag=Join-Path $run 'fail-uat.flag'
$buildMock='@echo off' + "`r`n" + 'echo BUILD %*>>"' + $capture + '"' + "`r`nexit /b 0"
$buildMock | Set-Content -LiteralPath (Join-Path $batch 'Build.bat') -Encoding ASCII
$uatMock=@'
@echo off
echo UAT %*>>"__CAPTURE__"
if exist "__FLAG__" exit /b 9
:next
if "%~1"=="" goto done
set "arg=%~1"
if "%arg:~0,18%"=="-archivedirectory=" set "archive=%arg:~18%"
shift
goto next
:done
if not defined archive exit /b 8
mkdir "%archive%"
echo fake executable>"%archive%\BorrowedSun.exe"
exit /b 0
'@
$uatMock.Replace('__CAPTURE__',$capture).Replace('__FLAG__',$flag) | Set-Content -LiteralPath (Join-Path $batch 'RunUAT.bat') -Encoding ASCII
$config=Join-Path $run 'fixture-config.json'
$relative=$projectDir.Substring($workspace.Length + 1).Replace('\','/')
$artifactRelative=$run.Substring($workspace.Length + 1).Replace('\','/') + '/Build Outputs'
Write-JsonFile $config @{SchemaVersion=1;EngineRoot=$mock;ProjectPath=($relative + '/BorrowedSun.uproject');ArtifactRoot=$artifactRelative;MapsToCook=@('/Game/Maps/Smoke')}
$dry=& "$toolsRoot/Build.ps1" -ConfigPath $config -Mode Package | ConvertFrom-Json
Assert-True ($dry.DryRun -and -not (Test-Path -LiteralPath (Join-Path $run 'Build Outputs'))) 'Dry run wrote output or did not identify itself.'; Pass
& "$toolsRoot/Build.ps1" -ConfigPath $config -Mode Package -Execute
$calls=Get-Content -LiteralPath $capture -Raw
Assert-True ($calls.Contains('BorrowedSunEditor') -and $calls.Contains('BorrowedSun') -and $calls.Contains('-map=/Game/Maps/Smoke') -and $calls.Contains('-MaxParallelActions=2')) 'Build/UAT arguments missing.'; Pass
'fail' | Set-Content -LiteralPath $flag -Encoding ASCII
Expect-Failure { & "$toolsRoot/Build.ps1" -ConfigPath $config -Mode Package -Execute } 'UAT failure did not propagate.'
Write-JsonFile $config @{SchemaVersion=1;EngineRoot=$mock;ProjectPath=($relative + '/BorrowedSun.uproject');ArtifactRoot=$artifactRelative;MapsToCook=@()}
Expect-Failure { & "$toolsRoot/Build.ps1" -ConfigPath $config -Mode Package } 'Package accepted no map.'
Write-JsonFile $config @{SchemaVersion=1;EngineRoot=$mock;ProjectPath=($relative + '/BorrowedSun.uproject');ArtifactRoot=$artifactRelative;MapsToCook=@('/Game/Maps/Smoke')}
'[/Script/EngineSettings.GameMapsSettings]','GameDefaultMap=/Game/Maps/Missing' | Set-Content -LiteralPath (Join-Path $projectDir 'Config/DefaultEngine.ini') -Encoding ASCII
Expect-Failure { & "$toolsRoot/Build.ps1" -ConfigPath $config -Mode Package } 'Package accepted a default map outside the cooked map set.'

# Hash checker accepts exact bytes, rejects changed bytes and escaping paths.
$snapshot=Join-Path $run 'Snapshot'
$null=New-Item -ItemType Directory -Path $snapshot
$data=Join-Path $snapshot 'source.txt'
'original' | Set-Content -LiteralPath $data -Encoding ASCII
$manifest=@{SchemaVersion=1;Complete=$true;Files=@(@{Path='source.txt';Bytes=(Get-Item -LiteralPath $data).Length;SHA256=(Get-FileHash -LiteralPath $data -Algorithm SHA256).Hash})}
Write-JsonFile (Join-Path $snapshot 'backup-manifest.json') $manifest
& "$toolsRoot/VerifyBackup.ps1" -SnapshotPath $snapshot | Out-Null; Pass
'tampered' | Set-Content -LiteralPath $data -Encoding ASCII
Expect-Failure { & "$toolsRoot/VerifyBackup.ps1" -SnapshotPath $snapshot } 'Tampered backup accepted.'
$manifest.Files[0].Path='../outside.txt'
Write-JsonFile (Join-Path $snapshot 'backup-manifest.json') $manifest
Expect-Failure { & "$toolsRoot/VerifyBackup.ps1" -SnapshotPath $snapshot } 'Manifest traversal accepted.'

# Full backup path in a disposable local repository; no remote or network exists.
$fixtureRepo=Join-Path $run 'Fixture Repo'
$fixtureTools=Join-Path $fixtureRepo 'Tools'
$null=New-Item -ItemType Directory -Path $fixtureTools
foreach ($name in @('Common.ps1','Environment.example.json','Backup.ps1','VerifyBackup.ps1')) { Copy-Item -LiteralPath (Join-Path $toolsRoot $name) -Destination (Join-Path $fixtureTools $name) }
'secret.local' | Set-Content -LiteralPath (Join-Path $fixtureRepo '.gitignore') -Encoding ASCII
'excluded local data' | Set-Content -LiteralPath (Join-Path $fixtureRepo 'secret.local') -Encoding ASCII
'committed source' | Set-Content -LiteralPath (Join-Path $fixtureRepo 'source.txt') -Encoding ASCII
$null=Invoke-CheckedProcess 'git' @('init',$fixtureRepo) $workspace
$null=Invoke-CheckedProcess 'git' @('-C',$fixtureRepo,'add','.') $workspace
$emptyHooks=Join-Path $run 'Empty Hooks'
$null=New-Item -ItemType Directory -Path $emptyHooks
$null=Invoke-CheckedProcess 'git' @('-C',$fixtureRepo,'-c','user.name=Tooling Fixture','-c','user.email=fixture@example.invalid','-c',('core.hooksPath=' + $emptyHooks),'-c','commit.gpgsign=false','commit','-m','Local tooling fixture') $workspace
'working addition' | Set-Content -LiteralPath (Join-Path $fixtureRepo 'untracked.txt') -Encoding ASCII
$backupDestination=Join-Path $run 'Backup Destination'
$null=New-Item -ItemType Directory -Path $backupDestination
$backupDry=& "$fixtureTools/Backup.ps1" -BackupRoot $backupDestination | ConvertFrom-Json
Assert-True ($backupDry.DryRun -and @(Get-ChildItem -LiteralPath $backupDestination).Count -eq 0) 'Backup dry run wrote files.'; Pass
& "$fixtureTools/Backup.ps1" -BackupRoot $backupDestination -Execute | Out-Null
$completed=@(Get-ChildItem -LiteralPath $backupDestination -Directory)
Assert-True ($completed.Count -eq 1) 'Backup snapshot missing.'
$backupManifest=Get-Content -LiteralPath (Join-Path $completed[0].FullName 'backup-manifest.json') -Raw | ConvertFrom-Json
Assert-True (($backupManifest.Files.Path -contains 'WorkingTree/untracked.txt') -and ($backupManifest.Files.Path -notcontains 'WorkingTree/secret.local') -and ($backupManifest.Files.Path -contains 'repository.bundle')) 'Backup source selection or bundle failed.'; Pass
Expect-Failure { & "$fixtureTools/Backup.ps1" -BackupRoot $fixtureRepo -Execute } 'Backup into source accepted.'

$summary=@{Passed=$passed; Failed=0; PowerShell=$PSVersionTable.PSVersion.ToString(); Scope='Tooling regression tests with mock engine; not Unreal gameplay or real cook validation'}
Write-JsonFile (Join-Path $run 'tooling-test-result.json') $summary
$summary | ConvertTo-Json
Write-Host ('Fixtures retained for inspection: ' + $run)
