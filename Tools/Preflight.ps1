[CmdletBinding()]
param([Alias('UE_ROOT')][string]$EngineRoot,[string]$ConfigPath,[switch]$ForPackage,[switch]$RequireClean)
. "$PSScriptRoot/Common.ps1"
$ctx=Get-WorkspaceContext -ConfigPath $ConfigPath -EngineRoot $EngineRoot
Assert-Project $ctx
if ($ctx.Config.EngineRoot) { $null=Assert-EngineLock $ctx }
foreach ($script in (Get-ChildItem -LiteralPath $PSScriptRoot -Filter '*.ps1' -File)) {
    $tokens=$null; $parseErrors=$null
    $null=[Management.Automation.Language.Parser]::ParseFile($script.FullName,[ref]$tokens,[ref]$parseErrors)
    if ($parseErrors.Count -gt 0) { throw ('PowerShell parse failure in ' + $script.Name) }
}
$content=Join-Path (Split-Path $ctx.Project -Parent) 'Content'
if (Test-Path -LiteralPath $content) {
    $pending=New-Object Collections.Generic.Queue[string]; $pending.Enqueue($content)
    while ($pending.Count -gt 0) {
        $directory=$pending.Dequeue(); Assert-NoReparsePoint $directory
        foreach ($item in (Get-ChildItem -LiteralPath $directory -Force)) {
            Assert-NoReparsePoint $item.FullName
            if ($item.PSIsContainer) { $pending.Enqueue($item.FullName) }
            elseif ($item.Extension -in @('.uasset','.umap')) { Assert-MaterializedAsset $item.FullName }
        }
    }
}
if ($ForPackage) { $null=Get-CookMaps $ctx }
$identity=Get-GitIdentity $ctx.Root
if ($RequireClean -and (-not $identity.Repository -or $identity.Dirty)) { throw 'A clean committed repository is required.' }
[pscustomobject]@{Passed=$true; Scope='Configuration, project JSON, script syntax, local asset pointers and optional maps/clean Git'; Git=$identity; Note='Does not prove gameplay, license clearance or cook success.'} | ConvertTo-Json -Depth 5
