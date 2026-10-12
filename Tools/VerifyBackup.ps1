[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$SnapshotPath)
. "$PSScriptRoot/Common.ps1"
$root=[IO.Path]::GetFullPath($SnapshotPath)
Assert-NoReparsePoint $root
$manifestPath=Join-Path $root 'backup-manifest.json'
if (-not (Test-Path -LiteralPath $manifestPath -PathType Leaf)) { throw 'No completed backup manifest exists.' }
$manifest=Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($manifest.SchemaVersion -ne 1 -or $manifest.Complete -ne $true -or @($manifest.Files).Count -eq 0) { throw 'Incomplete or unsupported backup manifest.' }
$seen=@{}
foreach ($file in $manifest.Files) {
    if ($seen.ContainsKey($file.Path)) { throw 'Duplicate path in backup manifest.' }; $seen[$file.Path]=$true
    $path=Resolve-ContainedPath $root $file.Path
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw ('Backup file missing: ' + $file.Path) }
    if ((Get-Item -LiteralPath $path).Length -ne $file.Bytes -or (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -cne $file.SHA256) { throw ('Backup verification failed: ' + $file.Path) }
}
[pscustomobject]@{Verified=$true; Files=@($manifest.Files).Count; Scope='Manifest file bytes only'; RestoreDrillRequired=$true; Note='Hashes detect accidental change, not adversarial tampering; this is not a signed manifest or a complete historical LFS audit.'} | ConvertTo-Json
