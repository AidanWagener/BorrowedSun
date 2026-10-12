[CmdletBinding()]
param([ValidateSet('Compile','Package')][string]$Mode='Compile', [Alias('UE_ROOT')][string]$EngineRoot, [string]$ConfigPath, [switch]$Execute)
. "$PSScriptRoot/Common.ps1"
$ctx = Get-WorkspaceContext -ConfigPath $ConfigPath -EngineRoot $EngineRoot
Assert-Project $ctx
$maps = @(); if ($Mode -eq 'Package') { $maps = @(Get-CookMaps $ctx) }
$cfg = $ctx.Config
$engine = $cfg.EngineRoot; if (-not $engine) { $engine = '<SET-ENGINE-ROOT>' }
$build = Join-Path $engine 'Engine/Build/BatchFiles/Build.bat'
$uat = Join-Path $engine 'Engine/Build/BatchFiles/RunUAT.bat'
$commonArgs = @($cfg.BuildPlatform,$cfg.BuildConfiguration,('-Project=' + $ctx.Project),'-WaitMutex','-NoHotReloadFromIDE',('-MaxParallelActions=' + $cfg.MaxParallelActions))
$steps = @(
    @{Name='editor'; File=$build; Args=@($cfg.EditorTarget) + $commonArgs},
    @{Name='game'; File=$build; Args=@($cfg.GameTarget) + $commonArgs}
)
# The editor target is always Development even when the game is Shipping.
$steps[0].Args[2] = 'Development'
if (-not $Execute) {
    [pscustomobject]@{DryRun=$true; Mode=$Mode; Steps=$steps; PackageMaps=$maps; PackageTool=$uat; Note='Nothing executed. Package uses a fresh Artifacts/build directory.'} | ConvertTo-Json -Depth 8
    return
}
if (-not $cfg.EngineRoot) { throw 'Set EngineRoot in Environment.local.json or pass -UE_ROOT.' }
if (-not (Test-Path -LiteralPath $build -PathType Leaf)) { throw 'Engine Build.bat is missing.' }
if ($Mode -eq 'Package' -and -not (Test-Path -LiteralPath $uat -PathType Leaf)) { throw 'Engine RunUAT.bat is missing.' }
$engineIdentity=Assert-EngineLock $ctx
$run = New-RunDirectory $ctx 'build'
$result = @{SchemaVersion=1; Status='Running'; Kind=$Mode; Project=$cfg.ProjectPath; Platform=$cfg.BuildPlatform; Configuration=$cfg.BuildConfiguration; Maps=$maps; Engine=$engineIdentity; Git=(Get-GitIdentity $ctx.Root); StartedUtc=[DateTime]::UtcNow.ToString('o')}
try {
    foreach ($step in $steps) {
        Write-Host ('Building ' + $step.Name + ' target...')
        $null = Invoke-CheckedProcess -FilePath $step.File -Arguments $step.Args -WorkingDirectory $ctx.Root -TimeoutSeconds ([int]$cfg.TimeoutMinutes * 60) -LogPath (Join-Path $run ($step.Name + '.log'))
    }
    if ($Mode -eq 'Package') {
        $archive = Join-Path $run 'Package'
        $argsUat = @('BuildCookRun','-nop4',('-project=' + $ctx.Project),'-build','-cook','-stage','-package','-pak','-iostore','-archive',('-archivedirectory=' + $archive),('-platform=' + $cfg.BuildPlatform),('-clientconfig=' + $cfg.BuildConfiguration),('-map=' + ($maps -join '+')),'-utf8output','-unattended',('-UbtArgs=-MaxParallelActions=' + $cfg.MaxParallelActions))
        $null = Invoke-CheckedProcess -FilePath $uat -Arguments $argsUat -WorkingDirectory $ctx.Root -TimeoutSeconds ([int]$cfg.TimeoutMinutes * 60) -LogPath (Join-Path $run 'package.log')
        $executables = @(Get-ChildItem -LiteralPath $archive -Filter ($cfg.GameTarget + '.exe') -Recurse -File -ErrorAction Stop)
        if ($executables.Count -eq 0) { throw 'UAT returned success without the expected project executable.' }
        $result.PackageExecutableCount = $executables.Count
    }
    $result.Status='Passed'; Write-Host ('Build passed. Evidence: ' + $run)
} catch { $result.Status='Failed'; throw }
finally { $result.FinishedUtc=[DateTime]::UtcNow.ToString('o'); Write-JsonFile (Join-Path $run 'build-result.json') $result }
