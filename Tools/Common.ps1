# Compatible with Windows PowerShell 5.1 and PowerShell 7 on Windows.
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

function Resolve-ContainedPath {
    param([string]$Root, [string]$Relative)
    if ([string]::IsNullOrWhiteSpace($Relative) -or [IO.Path]::IsPathRooted($Relative)) { throw 'Expected a nonempty relative path.' }
    $base = [IO.Path]::GetFullPath($Root).TrimEnd('\','/')
    $path = [IO.Path]::GetFullPath((Join-Path $base $Relative))
    if (-not $path.StartsWith($base + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw 'Path escapes its allowed root.' }
    Assert-NoReparsePoint $path
    return $path
}
function Assert-NoReparsePoint {
    param([string]$Path)
    $cursor = [IO.Path]::GetFullPath($Path)
    while ($cursor) {
        if (Test-Path -LiteralPath $cursor) {
            if (((Get-Item -LiteralPath $cursor -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse points are not supported by these workspace scripts.' }
        }
        $parent = [IO.Directory]::GetParent($cursor)
        if ($null -eq $parent) { break }
        $cursor = $parent.FullName
    }
}
function Get-WorkspaceContext {
    param([string]$ConfigPath, [string]$EngineRoot, [string]$WorkspaceRoot)
    if (-not $WorkspaceRoot) { $WorkspaceRoot = Split-Path $PSScriptRoot -Parent }
    $root = [IO.Path]::GetFullPath($WorkspaceRoot)
    $example = Join-Path $PSScriptRoot 'Environment.example.json'
    $cfg = @{}
    (Get-Content -LiteralPath $example -Raw | ConvertFrom-Json).PSObject.Properties | ForEach-Object { $cfg[$_.Name] = $_.Value }
    if (-not $ConfigPath) { $ConfigPath = Join-Path $PSScriptRoot 'Environment.local.json' }
    elseif (-not (Test-Path -LiteralPath $ConfigPath -PathType Leaf)) { throw 'Explicit configuration file does not exist.' }
    if (Test-Path -LiteralPath $ConfigPath -PathType Leaf) {
        (Get-Content -LiteralPath $ConfigPath -Raw | ConvertFrom-Json).PSObject.Properties | ForEach-Object {
            if (-not $cfg.ContainsKey($_.Name)) { throw ('Unknown configuration key: ' + $_.Name) }
            $cfg[$_.Name] = $_.Value
        }
    }
    if ($cfg.SchemaVersion -ne 1) { throw 'Unsupported configuration schema.' }
    if ($EngineRoot) { $cfg.EngineRoot = $EngineRoot }
    elseif (-not $cfg.EngineRoot -and $env:UE_ROOT) { $cfg.EngineRoot = $env:UE_ROOT }
    if ($cfg.EngineRoot) { $cfg.EngineRoot = [IO.Path]::GetFullPath($cfg.EngineRoot); Assert-NoReparsePoint $cfg.EngineRoot }
    foreach ($key in @('GameTarget','EditorTarget','BuildPlatform','BuildConfiguration')) {
        if ([string]$cfg[$key] -notmatch '^[A-Za-z0-9_]+$') { throw ('Invalid ' + $key) }
    }
    if ($cfg.BuildPlatform -ne 'Win64') { throw 'This initial runner supports Win64 only.' }
    if ($cfg.BuildConfiguration -notin @('Development','Shipping','DebugGame')) { throw 'Unsupported build configuration.' }
    if ([int]$cfg.MaxParallelActions -lt 1 -or [int]$cfg.MaxParallelActions -gt 64) { throw 'MaxParallelActions must be 1..64.' }
    if ([int]$cfg.TimeoutMinutes -lt 1 -or [int]$cfg.TimeoutMinutes -gt 720) { throw 'TimeoutMinutes must be 1..720.' }
    if ([int]$cfg.MinimumTests -lt 1) { throw 'MinimumTests must be at least one.' }
    if ([string]$cfg.TestFilter -notmatch '^[A-Za-z0-9_.+]+$') { throw 'TestFilter must contain only test identifiers, dots and plus signs.' }
    return [pscustomobject]@{ Root=$root; Config=$cfg; Project=(Resolve-ContainedPath $root $cfg.ProjectPath); Artifacts=(Resolve-ContainedPath $root $cfg.ArtifactRoot) }
}
function Assert-Project {
    param($Context)
    if (-not (Test-Path -LiteralPath $Context.Project -PathType Leaf)) { throw 'Project .uproject is missing.' }
    $null = Get-Content -LiteralPath $Context.Project -Raw | ConvertFrom-Json
}
function Assert-EngineLock {
    param($Context)
    $lockPath=Join-Path $Context.Root 'Config/engine-lock.json'
    if (-not (Test-Path -LiteralPath $lockPath -PathType Leaf)) { throw 'Reviewed Config/engine-lock.json is missing.' }
    $lock=Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
    $versionPath=Join-Path $Context.Config.EngineRoot 'Engine/Build/Build.version'
    if (-not (Test-Path -LiteralPath $versionPath -PathType Leaf)) { throw 'Engine Build.version is missing.' }
    $installed=Get-Content -LiteralPath $versionPath -Raw | ConvertFrom-Json
    $version=('{0}.{1}.{2}' -f $installed.MajorVersion,$installed.MinorVersion,$installed.PatchVersion)
    if ($version -cne $lock.engine.version -or $installed.Changelist -ne $lock.engine.changelist) { throw 'Installed engine version/changelist differs from the reviewed engine lock. Review an upgrade separately.' }
    return @{Version=$version; Changelist=$installed.Changelist}
}
function Get-CookMaps {
    param($Context)
    $maps = @($Context.Config.MapsToCook)
    if ($maps.Count -eq 0) { throw 'Packaging requires MapsToCook entries and saved .umap files; no map is inferred.' }
    foreach ($map in $maps) {
        if ($map -notmatch '^/Game/[A-Za-z0-9_/]+$') { throw 'MapsToCook entries must be /Game/Package paths without file extensions.' }
        $relative = 'Content/' + $map.Substring(6) + '.umap'
        $file = Resolve-ContainedPath (Split-Path $Context.Project -Parent) $relative
        if (-not (Test-Path -LiteralPath $file -PathType Leaf)) { throw ('Cook map does not exist: ' + $map) }
        Assert-MaterializedAsset $file
    }
    $ini=Join-Path (Split-Path $Context.Project -Parent) 'Config/DefaultEngine.ini'
    if (-not (Test-Path -LiteralPath $ini -PathType Leaf)) { throw 'Packaging requires DefaultEngine.ini with an explicit GameDefaultMap.' }
    $section=''; $defaults=@()
    foreach ($line in (Get-Content -LiteralPath $ini)) {
        $trimmed=$line.Trim()
        if ($trimmed -match '^\[(.+)\]$') { $section=$Matches[1]; continue }
        if ($section -eq '/Script/EngineSettings.GameMapsSettings' -and $trimmed -match '^GameDefaultMap\s*=\s*(.+?)\s*$') { $defaults += $Matches[1].Trim('"') }
    }
    if ($defaults.Count -ne 1) { throw 'Set exactly one GameDefaultMap in [/Script/EngineSettings.GameMapsSettings].' }
    $defaultPackage=($defaults[0] -split '\.')[0]
    if ($defaultPackage -notmatch '^/Game/[A-Za-z0-9_/]+$' -or $maps -cnotcontains $defaultPackage) { throw 'GameDefaultMap must be a project map included in MapsToCook.' }
    return $maps
}
function Assert-MaterializedAsset {
    param([string]$Path)
    $stream = [IO.File]::OpenRead($Path)
    try { $bytes = New-Object byte[] 128; $count = $stream.Read($bytes,0,128); $prefix = [Text.Encoding]::ASCII.GetString($bytes,0,$count) }
    finally { $stream.Dispose() }
    if ($prefix.StartsWith('version https://git-lfs.github.com/spec/v1')) { throw 'Unmaterialized Git LFS pointer found; fetch/checkout assets deliberately before building.' }
}
function ConvertTo-NativeArgument {
    param([AllowEmptyString()][string]$Value)
    if ($Value -notmatch '[\s"]' -and $Value.Length -gt 0) { return $Value }
    $escaped = [regex]::Replace($Value, '(\\*)"', '$1$1\"')
    $escaped = [regex]::Replace($escaped, '(\\+)$', '$1$1')
    return '"' + $escaped + '"'
}
function Invoke-WorkspaceProcess {
    param([string]$FilePath, [string[]]$Arguments=@(), [string]$WorkingDirectory, [int]$TimeoutSeconds=3600, [string]$LogPath)
    $psi = New-Object Diagnostics.ProcessStartInfo
    $psi.UseShellExecute = $false; $psi.CreateNoWindow = $true
    $psi.RedirectStandardOutput = $true; $psi.RedirectStandardError = $true
    $psi.WorkingDirectory = $WorkingDirectory
    if ([IO.Path]::GetExtension($FilePath) -in @('.bat','.cmd')) {
        # cmd.exe expansion is not the same as native argv quoting. Reject its metacharacters.
        foreach ($value in (@($FilePath) + $Arguments)) {
            if ($value -match '[&|<>^%!"\r\n]') { throw 'Batch command data contains unsupported shell metacharacters; use a conventional workspace path.' }
        }
        $parts = @('"' + $FilePath + '"') + @($Arguments | ForEach-Object { '"' + $_ + '"' })
        $psi.FileName = $env:ComSpec
        $psi.Arguments = '/d /s /c "' + ($parts -join ' ') + '"'
    } else {
        $psi.FileName = $FilePath
        $psi.Arguments = (@($Arguments | ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
    }
    $process = New-Object Diagnostics.Process
    $process.StartInfo = $psi
    try {
        if (-not $process.Start()) { throw 'Process did not start.' }
        $stdoutTask = $process.StandardOutput.ReadToEndAsync()
        $stderrTask = $process.StandardError.ReadToEndAsync()
        if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
            # Kill only the process tree launched here. No filesystem deletion or name-based process kill.
            $killInfo=New-Object Diagnostics.ProcessStartInfo
            $killInfo.FileName="$env:SystemRoot\System32\taskkill.exe"
            $killInfo.Arguments=('/PID ' + $process.Id + ' /T /F')
            $killInfo.UseShellExecute=$false; $killInfo.CreateNoWindow=$true
            $killer=[Diagnostics.Process]::Start($killInfo)
            try { if (-not $killer.WaitForExit(5000)) { $killer.Kill() } } finally { $killer.Dispose() }
            $stopped=$process.WaitForExit(5000)
            $stdout='[stdout capture incomplete after bounded timeout cleanup]'
            $stderr='[stderr capture incomplete after bounded timeout cleanup]'
            if ($stdoutTask.Wait(2000)) { $stdout=$stdoutTask.GetAwaiter().GetResult() }
            if ($stderrTask.Wait(2000)) { $stderr=$stderrTask.GetAwaiter().GetResult() }
            if ($LogPath) { [IO.File]::WriteAllText($LogPath, $stdout + [Environment]::NewLine + $stderr + [Environment]::NewLine + ('Timeout cleanup observed process exit: ' + $stopped), (New-Object Text.UTF8Encoding($false))) }
            throw ('Process timed out after ' + $TimeoutSeconds + ' seconds.')
        }
        if (-not $stdoutTask.Wait(5000) -or -not $stderrTask.Wait(5000)) { throw 'Process exited but redirected streams did not close within the bounded capture wait.' }
        $stdout = $stdoutTask.GetAwaiter().GetResult(); $stderr = $stderrTask.GetAwaiter().GetResult()
        if ($LogPath) { [IO.File]::WriteAllText($LogPath, $stdout + [Environment]::NewLine + $stderr, (New-Object Text.UTF8Encoding($false))) }
        return [pscustomobject]@{ ExitCode=$process.ExitCode; StdOut=$stdout; StdErr=$stderr }
    } finally { $process.Dispose() }
}
function Invoke-CheckedProcess {
    param([string]$FilePath,[string[]]$Arguments=@(),[string]$WorkingDirectory,[int]$TimeoutSeconds=3600,[string]$LogPath)
    $result = Invoke-WorkspaceProcess @PSBoundParameters
    if ($result.ExitCode -ne 0) { throw ('Native command failed with exit code ' + $result.ExitCode + '. Inspect its local log.') }
    return $result
}
function New-RunDirectory {
    param($Context,[string]$Kind)
    $id = [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ') + '-' + [Guid]::NewGuid().ToString('N').Substring(0,8)
    $path = Resolve-ContainedPath $Context.Artifacts ($Kind + '/' + $id)
    $null = New-Item -ItemType Directory -Path $path
    return $path
}
function Write-JsonFile {
    param([string]$Path,$Value)
    [IO.File]::WriteAllText($Path, ($Value | ConvertTo-Json -Depth 20), (New-Object Text.UTF8Encoding($false)))
}
function Get-GitIdentity {
    param([string]$Root)
    try {
        $head = Invoke-CheckedProcess -FilePath 'git' -Arguments @('-C',$Root,'rev-parse','HEAD') -WorkingDirectory $Root
        $status = Invoke-CheckedProcess -FilePath 'git' -Arguments @('-C',$Root,'status','--porcelain') -WorkingDirectory $Root
        return @{Commit=$head.StdOut.Trim(); Dirty=([bool]$status.StdOut); Repository=$true}
    } catch { return @{Commit=$null; Dirty=$null; Repository=$false} }
}
function Test-AutomationReport {
    param([string]$ReportPath,[int]$MinimumTests=1,[string[]]$ExpectedTests=@())
    if (-not (Test-Path -LiteralPath $ReportPath -PathType Leaf)) { throw 'Automation report is missing.' }
    $report = Get-Content -LiteralPath $ReportPath -Raw | ConvertFrom-Json
    if (-not ($report.PSObject.Properties.Name -contains 'tests')) { throw 'Automation JSON schema lacks tests array.' }
    $tests = @($report.tests)
    if ($tests.Count -lt $MinimumTests) { throw ('Insufficient tests: ' + $tests.Count + '; expected at least ' + $MinimumTests) }
    foreach ($test in $tests) {
        if (-not ($test.PSObject.Properties.Name -contains 'state') -or $test.state -cne 'Success') { throw 'Automation has a failed, incomplete, skipped or unknown test state.' }
    }
    foreach ($expected in $ExpectedTests) {
        $match = @($tests | Where-Object { $_.PSObject.Properties.Name -contains 'fullTestPath' -and $_.fullTestPath -ceq $expected })
        if ($match.Count -ne 1) { throw ('Expected test absent or duplicated: ' + $expected) }
    }
    foreach ($field in @('failed','notRun','inProcess')) {
        if ($report.PSObject.Properties.Name -contains $field) { if ([int]$report.$field -ne 0) { throw ('Automation summary reports ' + $field) } }
    }
    return [pscustomobject]@{ Passed=$true; Count=$tests.Count; ExpectedCount=$ExpectedTests.Count }
}
