[CmdletBinding()]
param([Alias('UE_ROOT')][string]$EngineRoot,[string]$ConfigPath,[switch]$RequireEngine)
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath -EngineRoot $EngineRoot
$checks=New-Object Collections.Generic.List[object]
$checks.Add([pscustomobject]@{Name='PowerShell'; Ready=($PSVersionTable.PSVersion.Major -ge 5); Detail=$PSVersionTable.PSVersion.ToString()})
$checks.Add([pscustomobject]@{Name='Project'; Ready=(Test-Path -LiteralPath $ctx.Project -PathType Leaf); Detail=$ctx.Config.ProjectPath})
foreach ($command in @('git','git-lfs')) {
    $available=[bool](Get-Command $command -ErrorAction SilentlyContinue)
    $checks.Add([pscustomobject]@{Name=$command; Ready=$available; Detail='Availability only; nothing installed or configured'})
}
$engineOK=$false
if ($ctx.Config.EngineRoot) { $engineOK=Test-Path -LiteralPath (Join-Path $ctx.Config.EngineRoot 'Engine/Build/BatchFiles/Build.bat') -PathType Leaf }
$checks.Add([pscustomobject]@{Name='Unreal'; Ready=$engineOK; Detail='Explicit EngineRoot required for execution'})
$checks | Format-Table -AutoSize
Write-Host 'Advisory: keep Editor closed while compiling. MaxParallelActions is configurable; leave RAM and disk headroom for cook, cache and package outputs.'
Write-Host 'This check does not install SDKs, authenticate services, reveal remote URLs or prove a successful build.'
if ($RequireEngine -and -not $engineOK) { throw 'Engine readiness required but not satisfied.' }
if ($RequireEngine) { $null=Assert-EngineLock $ctx }
