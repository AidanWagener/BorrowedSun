[CmdletBinding()]
param([string]$BackupRoot,[string]$ConfigPath,[switch]$Execute)
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath
if (-not $BackupRoot) { $BackupRoot=$ctx.Config.BackupRoot }
if (-not $BackupRoot) { throw 'Choose an existing backup destination outside this workspace with -BackupRoot or local configuration.' }
$destination=[IO.Path]::GetFullPath($BackupRoot).TrimEnd('\','/')
Assert-NoReparsePoint $destination
if ($destination -eq $ctx.Root -or $destination.StartsWith($ctx.Root.TrimEnd('\','/') + '\',[StringComparison]::OrdinalIgnoreCase)) { throw 'Backup destination must be outside the source workspace.' }
if (-not (Test-Path -LiteralPath $destination -PathType Container)) { throw 'Backup destination must already exist; this script creates only a new snapshot beneath it.' }
$identity=Get-GitIdentity $ctx.Root
if (-not $identity.Repository) { throw 'Backup requires a Git repository with at least one commit. First commit reviewed source; the script will not initialize it.' }
$listing=Invoke-CheckedProcess -FilePath 'git' -Arguments @('-C',$ctx.Root,'ls-files','--cached','--others','--exclude-standard','-z') -WorkingDirectory $ctx.Root
$files=@($listing.StdOut.Split([char]0) | Where-Object { $_ } | Sort-Object -Unique)
$gitDirResult=Invoke-CheckedProcess -FilePath 'git' -Arguments @('-C',$ctx.Root,'rev-parse','--absolute-git-dir') -WorkingDirectory $ctx.Root
$lfsObjects=Join-Path $gitDirResult.StdOut.Trim() 'lfs/objects'
if (-not $Execute) {
    [pscustomobject]@{DryRun=$true; SourceFileCount=$files.Count; Dirty=$identity.Dirty; Includes='Current nonignored working files, full Git bundle, locally present LFS objects'; HistoryCoverage='Only LFS objects present locally; no fetch is performed'; Note='No files written. Stop editor and imports before executing. Destination path is intentionally omitted from report.'} | ConvertTo-Json
    return
}
$id=[DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ') + '-' + [Guid]::NewGuid().ToString('N').Substring(0,8)
$snapshot=Resolve-ContainedPath $destination ('BorrowedSun-' + $id)
$null=New-Item -ItemType Directory -Path $snapshot
$records=New-Object Collections.Generic.List[object]
function Copy-VerifiedSnapshotFile {
    param([string]$Source,[string]$Relative)
    Assert-NoReparsePoint $Source
    $target=Resolve-ContainedPath $snapshot $Relative
    $null=New-Item -ItemType Directory -Path (Split-Path $target -Parent) -Force
    $before=(Get-FileHash -LiteralPath $Source -Algorithm SHA256).Hash
    Copy-Item -LiteralPath $Source -Destination $target
    $after=(Get-FileHash -LiteralPath $Source -Algorithm SHA256).Hash
    $copy=(Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash
    if ($before -ne $after -or $before -ne $copy) { throw 'A source changed during backup or its copied bytes differ. Snapshot remains incomplete.' }
    $records.Add([pscustomobject]@{Path=$Relative.Replace('\','/'); Bytes=(Get-Item -LiteralPath $target).Length; SHA256=$copy})
}
foreach ($relative in $files) {
    $source=Resolve-ContainedPath $ctx.Root $relative
    if (Test-Path -LiteralPath $source -PathType Leaf) {
        if ([IO.Path]::GetExtension($source) -in @('.uasset','.umap')) { Assert-MaterializedAsset $source }
        Copy-VerifiedSnapshotFile $source ('WorkingTree/' + $relative)
    }
}
$bundle=Join-Path $snapshot 'repository.bundle'
$null=Invoke-CheckedProcess -FilePath 'git' -Arguments @('-C',$ctx.Root,'bundle','create',$bundle,'--all') -WorkingDirectory $ctx.Root
$null=Invoke-CheckedProcess -FilePath 'git' -Arguments @('-C',$ctx.Root,'bundle','verify',$bundle) -WorkingDirectory $ctx.Root
$records.Add([pscustomobject]@{Path='repository.bundle'; Bytes=(Get-Item -LiteralPath $bundle).Length; SHA256=(Get-FileHash -LiteralPath $bundle -Algorithm SHA256).Hash})
if (Test-Path -LiteralPath $lfsObjects -PathType Container) {
    $pending=New-Object Collections.Generic.Queue[string]; $pending.Enqueue($lfsObjects)
    while ($pending.Count -gt 0) {
        $directory=$pending.Dequeue(); Assert-NoReparsePoint $directory
        foreach ($item in (Get-ChildItem -LiteralPath $directory -Force)) {
            Assert-NoReparsePoint $item.FullName
            if ($item.PSIsContainer) { $pending.Enqueue($item.FullName) }
            else { Copy-VerifiedSnapshotFile $item.FullName ('LfsObjects/' + $item.FullName.Substring($lfsObjects.Length + 1)) }
        }
    }
}
# A changed file listing or commit invalidates the snapshot; editing must be paused for the run.
$endIdentity=Get-GitIdentity $ctx.Root
$endListing=Invoke-CheckedProcess -FilePath 'git' -Arguments @('-C',$ctx.Root,'ls-files','--cached','--others','--exclude-standard','-z') -WorkingDirectory $ctx.Root
if ($identity.Commit -ne $endIdentity.Commit -or $listing.StdOut -cne $endListing.StdOut) { throw 'Repository changed during backup; no completed manifest was written.' }
$manifest=@{SchemaVersion=1; Complete=$true; CreatedUtc=[DateTime]::UtcNow.ToString('o'); Git=$identity; HistoryCoverage='Local LFS objects only; historical completeness not asserted'; Files=@($records.ToArray())}
Write-JsonFile (Join-Path $snapshot 'backup-manifest.json') $manifest
& "$PSScriptRoot/VerifyBackup.ps1" -SnapshotPath $snapshot
Write-Host ('Verified snapshot: ' + $snapshot)
