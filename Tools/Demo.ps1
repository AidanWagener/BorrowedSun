<#
.SYNOPSIS
Preview or run the Borrowed Sun demo, its map authoring commandlet, or rendered route validation.
.DESCRIPTION
All modes are dry runs unless -Execute is supplied. Existing Tools environment configuration
and the reviewed engine lock apply. This script never builds, installs, downloads or deletes.
Launch uses ordinary player saves. Validate uses a fresh isolated QA save namespace and requires
both complete mission routes and their checkpoint checks in a fresh, rendered-runtime report.
.PARAMETER PackageExecutable
Existing packaged game executable. Supported for Launch and Validate only.
.PARAMETER ReportDirectory
For packaged Validate without UserDirectory, REQUIRED: the game's actual ProjectSavedDir/DemoValidation directory.
This selects where to READ the harness report; it does not redirect the game's output.
Editor validation defaults to Game/Saved/DemoValidation beside the configured project.
.PARAMETER UserDirectory
Optional player-data root for Launch or Validate, passed to Unreal as -UserDir. Saves and preferences
then live under its Saved subdirectory. Validate derives ReportDirectory from this root when omitted.
.PARAMETER Screenshots
Validate only: request and require all six route milestone PNGs. Images still need visual review.
.PARAMETER MeasureFrames
Validate only: collect application frame intervals after five seconds of warmup. Cannot be
combined with Screenshots because screenshot readback distorts timing. This is not GPU profiling.
.PARAMETER Force
AuthorMap only: explicitly permit the commandlet to replace its owned demo map and material.
.PARAMETER TimeoutSeconds
AuthorMap and Validate only: bounded native process lifetime, 10..420 seconds. Expiry terminates
only the launched process tree and fails the run. Launch returns a PID immediately and never
applies a lifetime limit; the player closes the game normally.
.EXAMPLE
powershell -NoProfile -File Tools/Demo.ps1
.EXAMPLE
powershell -NoProfile -File Tools/Demo.ps1 -Mode Launch -Execute
.EXAMPLE
powershell -NoProfile -File Tools/Demo.ps1 -Mode AuthorMap -Execute
.EXAMPLE
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -Screenshots -Execute
.EXAMPLE
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -PackageExecutable 'D:\Build\BorrowedSun.exe' -ReportDirectory 'D:\Build\BorrowedSun\Saved\DemoValidation' -Execute
.NOTES
Close any editor/game using this project before AuthorMap or Validate. Do not run them alongside
a build or another writer of the same assets. Rendered validation requires a desktop/GPU session;
it intentionally has no NullRHI option. It drives real gameplay methods, not physical input devices.
Evidence is local under Artifacts/demo/<fresh-run>. QA checkpoint files remain isolated for inspection.
Launch reports Launched, not Passed; it does not observe the eventual exit code or validate gameplay.
Use -UE_ROOT as an alias for -EngineRoot. Windows PowerShell 5.1 and PowerShell 7 are supported.
#>
[CmdletBinding()]
param(
    [ValidateSet('Launch','AuthorMap','Validate')][string]$Mode='Launch',
    [Alias('UE_ROOT')][string]$EngineRoot,
    [string]$ConfigPath,
    [string]$PackageExecutable,
    [string]$ReportDirectory,
    [string]$UserDirectory,
    [ValidateRange(10,420)][int]$TimeoutSeconds=420,
    [switch]$Screenshots,
    [switch]$MeasureFrames,
    [switch]$Force,
    [switch]$Execute
)
. "$PSScriptRoot/Common.ps1"

function Test-DemoRuntimeReport {
    param([string]$ReportPath,[string]$ExpectedNamespace,[datetime]$StartedUtc,[switch]$RequireScreenshots,[switch]$RequireFrameSamples)
    if (-not (Test-Path -LiteralPath $ReportPath -PathType Leaf)) { throw 'Demo validation report is missing; native exit zero is insufficient.' }
    Assert-NoReparsePoint $ReportPath
    $reportFile=Get-Item -LiteralPath $ReportPath
    if ($reportFile.LastWriteTimeUtc -lt $StartedUtc.ToUniversalTime().AddSeconds(-2)) { throw 'Demo validation report predates this run.' }
    $report=Get-Content -LiteralPath $ReportPath -Raw | ConvertFrom-Json
    foreach ($field in @('schemaVersion','namespace','success','canEverRender','nullRHIRequested','checks')) {
        if ($null -eq $report -or $report.PSObject.Properties.Name -cnotcontains $field) { throw ('Demo validation schema lacks ' + $field) }
    }
    if ($report.schemaVersion -ne 1) { throw 'Unsupported demo validation schema.' }
    if ($report.namespace -cne $ExpectedNamespace) { throw 'Demo validation namespace does not match this fresh run.' }
    foreach ($field in @('success','canEverRender')) {
        if ($report.$field -isnot [bool] -or $report.$field -ne $true) { throw ('Demo validation requires boolean true for ' + $field) }
    }
    if ($report.nullRHIRequested -isnot [bool] -or $report.nullRHIRequested -ne $false) { throw 'Demo validation requires a rendered run without NullRHI.' }
    if ($report.checks -isnot [array] -or $report.checks.Count -eq 0) { throw 'Demo validation contains no check array.' }
    foreach ($check in $report.checks) {
        if ($null -eq $check -or $check.PSObject.Properties.Name -cnotcontains 'name' -or $check.PSObject.Properties.Name -cnotcontains 'passed') { throw 'A demo check is malformed.' }
        if ($check.passed -isnot [bool] -or $check.passed -ne $true) { throw ('Demo check failed or incomplete: ' + $check.name) }
    }
    $required=@('Clinic.IsolatedSaveNamespace','Clinic.RenderedRuntimeRequired','Clinic.RuntimeReady')
    $routeChecks=@('ResetCheckpoint','FreshJourney','AcquireThroughInteraction','BoardThroughInteraction',
        'ReachRelayByFlight','AuthorizeSingleLease','ReturnAndDisembark','InstallThroughInteraction',
        'DuplicateInteractionHarmless','ManualCheckpointSaved','CheckpointReadFromDisk',
        'CompletedLeaseSurvivesLoad','RouteWithinBudget')
    foreach ($route in @('Clinic','Evacuation')) {
        foreach ($name in $routeChecks) { $required += $route + '.' + $name }
        if (@($report.checks | Where-Object { $_.name -ceq ($route + '.WalkingMakesProgress') }).Count -lt 1) { throw ('Missing walking evidence for ' + $route) }
    }
    foreach ($name in $required) {
        if (@($report.checks | Where-Object { $_.name -ceq $name }).Count -ne 1) { throw ('Required demo check absent or duplicated: ' + $name) }
    }
    $screenshotCount=0
    if ($RequireFrameSamples) {
        if ($report.PSObject.Properties.Name -cnotcontains 'frameIntervals') { throw 'Requested frame interval report is missing.' }
        $frames=$report.frameIntervals
        if ($null -eq $frames.frameCap -or [math]::Abs([double]$frames.frameCap-60.0) -gt .01) { throw 'Frame measurement did not use the required 60 FPS player target.' }
        if ($frames.overflow -isnot [bool] -or $frames.overflow -ne $false -or $report.screenshotsRequested -ne $false) { throw 'Frame samples overflowed or include screenshot capture.' }
        if ($frames.width -lt 640 -or $frames.height -lt 360 -or $frames.warmupSeconds -lt 5) { throw 'Frame samples lack valid viewport/warmup metadata.' }
        foreach ($mode in @('overall','onFoot','flight')) {
            $sample=$frames.$mode
            if ($sample.sampleCount -lt 60) { throw ('Insufficient frame interval samples for ' + $mode) }
            foreach ($field in @('sampleSeconds','meanMs','medianMs','p95Ms','p99Ms','maxMs')) {
                $number=$sample.$field
                if ($null -eq $number -or $number -is [string] -or $number -is [bool] -or [double]::IsNaN([double]$number) -or [double]::IsInfinity([double]$number) -or $number -le 0) { throw ('Invalid frame interval statistic: ' + $field) }
            }
            if ($sample.medianMs -gt $sample.p95Ms -or $sample.p95Ms -gt $sample.p99Ms -or $sample.p99Ms -gt $sample.maxMs) { throw 'Frame interval percentiles are inconsistent.' }
        }
        if ($frames.overall.sampleCount -ne ($frames.onFoot.sampleCount + $frames.flight.sampleCount)) { throw 'Frame interval sample counts do not reconcile.' }
    }
    if ($RequireScreenshots) {
        if ($report.PSObject.Properties.Name -cnotcontains 'screenshotsRequested' -or $report.screenshotsRequested -isnot [bool] -or $report.screenshotsRequested -ne $true) { throw 'Screenshot capture was not enabled in the runtime report.' }
        foreach ($route in @('Clinic','Evacuation')) {
            foreach ($milestone in @('street-start','sky-relay','service-complete')) {
                $png=Join-Path (Split-Path $ReportPath -Parent) ($ExpectedNamespace + '-' + $route + '-' + $milestone + '.png')
                if (-not (Test-Path -LiteralPath $png -PathType Leaf)) { throw ('Requested demo screenshot is missing: ' + $route + '.' + $milestone) }
                Assert-NoReparsePoint $png
                if ((Get-Item -LiteralPath $png).LastWriteTimeUtc -lt $StartedUtc.ToUniversalTime().AddSeconds(-2)) { throw 'Requested demo screenshot predates this run.' }
                $stream=[IO.File]::OpenRead($png)
                try { $header=New-Object byte[] 8; $length=$stream.Read($header,0,8) } finally { $stream.Dispose() }
                if ($length -ne 8 -or [BitConverter]::ToString($header) -cne '89-50-4E-47-0D-0A-1A-0A') { throw 'Requested demo screenshot has no PNG signature.' }
                $screenshotCount++
            }
        }
    }
    return [pscustomobject]@{Passed=$true; Count=$report.checks.Count; RequiredCount=$required.Count; ScreenshotCount=$screenshotCount}
}

function Start-DemoInteractiveProcess {
    param([string]$FilePath,[string[]]$Arguments,[string]$WorkingDirectory)
    $startInfo=New-Object Diagnostics.ProcessStartInfo
    $startInfo.FileName=$FilePath
    $startInfo.Arguments=(@($Arguments | ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
    $startInfo.WorkingDirectory=$WorkingDirectory
    $startInfo.UseShellExecute=$false
    # Suppresses an extra console, not the game's GUI window. No hidden WindowStyle is applied.
    $startInfo.CreateNoWindow=$true
    $process=New-Object Diagnostics.Process
    $process.StartInfo=$startInfo
    try {
        if (-not $process.Start()) { throw 'Interactive demo process did not start.' }
        return [pscustomobject]@{ProcessId=$process.Id; Status='Launched'}
    } finally {
        # Releasing our handle neither waits for nor terminates the independent player session.
        $process.Dispose()
    }
}

if ($Force -and $Mode -ne 'AuthorMap') { throw '-Force is supported only by AuthorMap.' }
if ($Screenshots -and $Mode -ne 'Validate') { throw '-Screenshots is supported only by Validate.' }
if ($MeasureFrames -and ($Mode -ne 'Validate' -or $Screenshots)) { throw '-MeasureFrames requires Validate without Screenshots.' }
if ($ReportDirectory -and $Mode -ne 'Validate') { throw '-ReportDirectory is supported only by Validate.' }
if ($PackageExecutable -and $Mode -eq 'AuthorMap') { throw 'AuthorMap requires the installed editor, not a packaged game.' }
if ($UserDirectory) {
    if ($Mode -eq 'AuthorMap') { throw '-UserDirectory is only supported by Launch or Validate.' }
    $UserDirectory=[IO.Path]::GetFullPath($UserDirectory)
    Assert-NoReparsePoint $UserDirectory
    if ($Mode -eq 'Validate') {
        $derivedReport=Join-Path $UserDirectory 'Saved/DemoValidation'
        if ($ReportDirectory -and [IO.Path]::GetFullPath($ReportDirectory) -ine [IO.Path]::GetFullPath($derivedReport)) { throw 'ReportDirectory does not match the selected UserDirectory.' }
        $ReportDirectory=$derivedReport
    }
}
if ($PackageExecutable -and $Mode -eq 'Validate' -and -not $ReportDirectory) { throw 'Packaged Validate requires -ReportDirectory or -UserDirectory identifying its actual saved-data directory.' }
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath -EngineRoot $EngineRoot
Assert-Project $ctx
$projectDirectory=Split-Path $ctx.Project -Parent
$map='/Game/BorrowedSun/Maps/L_BorrowedSunDemo'
$mapFile=Resolve-ContainedPath $projectDirectory 'Content/BorrowedSun/Maps/L_BorrowedSunDemo.umap'
$materialFile=Resolve-ContainedPath $projectDirectory 'Content/BorrowedSun/Art/M_DemoSurface.uasset'
$engine=$ctx.Config.EngineRoot
if (-not $engine) { $engine='<SET-ENGINE-ROOT>' }
$workingDirectory=$ctx.Root
if ($PackageExecutable) {
    $executable=[IO.Path]::GetFullPath($PackageExecutable)
    Assert-NoReparsePoint $executable
    if ([IO.Path]::GetExtension($executable) -ine '.exe' -or -not (Test-Path -LiteralPath $executable -PathType Leaf)) { throw 'PackageExecutable must be an existing game .exe.' }
    $arguments=@($map)
    $workingDirectory=Split-Path $executable -Parent
} elseif ($Mode -eq 'AuthorMap') {
    $executable=Join-Path $engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
    $arguments=@($ctx.Project,'-run=BSDemoCreateMap','-NullRHI','-unattended')
    if ($Force) { $arguments += '-Force' }
} else {
    $executable=Join-Path $engine 'Engine/Binaries/Win64/UnrealEditor.exe'
    $arguments=@($ctx.Project,'-game',$map)
}
$arguments += @('-nop4','-nosplash')
if ($UserDirectory) { $arguments += '-UserDir=' + $UserDirectory }
if ($Mode -ne 'Launch') { $arguments += @('-stdout','-FullStdOutLogOutput') }
if ($Mode -ne 'AuthorMap') { $arguments += @('-windowed','-ResX=1280','-ResY=720') }
$saveNamespace=$null; $reportPath=$null
if ($Mode -eq 'Validate') {
    $saveNamespace='QA_' + [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ') + '_' + [Guid]::NewGuid().ToString('N').Substring(0,16)
    if (-not $ReportDirectory) { $ReportDirectory=Resolve-ContainedPath $projectDirectory 'Saved/DemoValidation' }
    $ReportDirectory=[IO.Path]::GetFullPath($ReportDirectory)
    Assert-NoReparsePoint $ReportDirectory
    $reportPath=Join-Path $ReportDirectory ('Result-' + $saveNamespace + '.json')
    $arguments += @('-unattended','-BSDemoAutoTest',('-BSDemoSaveNamespace=' + $saveNamespace))
    if ($Screenshots) { $arguments += '-BSDemoScreenshots' }
    if ($MeasureFrames) { $arguments += '-BSDemoMeasureFrames' }
}
if (-not $Execute) {
    [pscustomobject]@{DryRun=$true; Mode=$Mode; Executable=$executable; Arguments=$arguments;
        TimeoutSeconds=$(if ($Mode -eq 'Launch') { $null } else { $TimeoutSeconds }); SaveNamespace=$saveNamespace; ExpectedReport=$reportPath;
        Screenshots=[bool]$Screenshots; MeasureFrames=[bool]$MeasureFrames; Force=[bool]$Force;
        Note='Nothing executed or written. Execute records fresh local Artifacts/demo evidence; Validate requires both rendered routes.'} | ConvertTo-Json -Depth 6
    return
}
if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) { throw 'Configured executable is missing.' }
$engineIdentity=$null
if (-not $PackageExecutable) {
    if (-not $ctx.Config.EngineRoot) { throw 'Set EngineRoot in Environment.local.json or pass -UE_ROOT.' }
    $engineIdentity=Assert-EngineLock $ctx
}
if ($Mode -ne 'AuthorMap' -and -not $PackageExecutable) {
    if (-not (Test-Path -LiteralPath $mapFile -PathType Leaf)) { throw 'The authored demo map is missing. Run AuthorMap after compiling the editor.' }
    Assert-MaterializedAsset $mapFile
}
if ($reportPath -and (Test-Path -LiteralPath $reportPath)) { throw 'Fresh namespace report already exists. Refusing to reuse it.' }
$run=New-RunDirectory $ctx 'demo'
$arguments += '-abslog=' + (Join-Path $run 'engine.log')
$result=@{SchemaVersion=1; Status='Running'; Mode=$Mode; Packaged=[bool]$PackageExecutable;
    SaveNamespace=$saveNamespace; Engine=$engineIdentity; ScreenshotsRequested=[bool]$Screenshots; FrameSamplesRequested=[bool]$MeasureFrames;
    Force=[bool]$Force; Git=(Get-GitIdentity $ctx.Root); StartedUtc=[DateTime]::UtcNow.ToString('o')}
$startedUtc=[DateTime]::UtcNow
try {
    if ($Mode -eq 'Launch') {
        $launched=Start-DemoInteractiveProcess -FilePath $executable -Arguments $arguments -WorkingDirectory $workingDirectory
        $result.Status='Launched'
        $result.ProcessId=$launched.ProcessId
        Write-Host ('Demo launched (PID ' + $launched.ProcessId + '). Close the game normally; no lifetime limit is applied. This is not gameplay validation.')
        Write-Host ('Launch evidence and engine log: ' + $run)
        return
    }
    $null=Invoke-CheckedProcess -FilePath $executable -Arguments $arguments -WorkingDirectory $workingDirectory -TimeoutSeconds $TimeoutSeconds -LogPath (Join-Path $run 'process.log')
    if ($Mode -eq 'Validate') {
        $validated=Test-DemoRuntimeReport -ReportPath $reportPath -ExpectedNamespace $saveNamespace -StartedUtc $startedUtc -RequireScreenshots:$Screenshots -RequireFrameSamples:$MeasureFrames
        Copy-Item -LiteralPath $reportPath -Destination (Join-Path $run 'runtime-result.json')
        $result.CheckCount=$validated.Count
        $result.RequiredCheckCount=$validated.RequiredCount
        $result.ScreenshotCount=$validated.ScreenshotCount
        $result.RuntimeReport=$reportPath
        Write-Host ('Rendered demo validation passed: ' + $validated.Count + ' checks across Clinic and Evacuation. Images still require visual review.')
    } elseif ($Mode -eq 'AuthorMap') {
        foreach ($asset in @($mapFile,$materialFile)) {
            if (-not (Test-Path -LiteralPath $asset -PathType Leaf) -or (Get-Item -LiteralPath $asset).Length -eq 0) { throw 'AuthorMap exited without both nonempty authored packages.' }
            Assert-MaterializedAsset $asset
        }
        Write-Host 'Demo map and surface material authored.'
    }
    $result.Status='Passed'
    Write-Host ('Evidence: ' + $run)
} catch { $result.Status='Failed'; throw }
finally { $result.FinishedUtc=[DateTime]::UtcNow.ToString('o'); Write-JsonFile (Join-Path $run 'demo-result.json') $result }
