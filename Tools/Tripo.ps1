<#
.SYNOPSIS
Plan, submit once, and resume one authored Tripo text-to-model brief.
.DESCRIPTION
Doctor and Plan are local-only. Generate requires -Execute and submits one task.
Resume queries the saved task once and downloads on success; it never generates.
An ambiguous submission remains unknown. Recover it only with a verified -TaskId.
No credentials are read by this adapter, and CLI stderr is never echoed or saved.
#>
[CmdletBinding()]
param(
    [ValidateSet('Doctor','Plan','Generate','Resume')][string]$Mode='Plan',
    [string]$BriefFile,
    [switch]$Execute,
    [string]$TaskId,
    [string]$CliPath,
    [ValidateRange(10,600)][int]$TimeoutSeconds=300
)
. "$PSScriptRoot/Common.ps1"
$workspace=Split-Path $PSScriptRoot -Parent

function Get-TripoField {
    param($Object,[string]$Name)
    if($null -ne $Object -and $Object.PSObject.Properties.Name -contains $Name){return $Object.$Name}
    return $null
}
function Assert-TripoInteger {
    param($Value,[long]$Min,[long]$Max,[string]$Name)
    if($null -eq $Value -or $Value -is [bool] -or $Value -is [string] -or $Value -isnot [ValueType]){throw "$Name must be an integer."}
    $number=[double]$Value
    if([double]::IsNaN($number) -or [double]::IsInfinity($number) -or $number -ne [math]::Truncate($number) -or $number -lt $Min -or $number -gt $Max){throw "$Name is outside the supported integer range."}
}
function Read-TripoBrief {
    param([string]$Path)
    if(-not $Path){throw '-BriefFile is required.'}
    $resolved=[IO.Path]::GetFullPath($Path);Assert-NoReparsePoint $resolved
    $brief=Get-Content -LiteralPath $resolved -Raw -Encoding UTF8 | ConvertFrom-Json
    $keys=@('schemaVersion','id','prompt','model','seed','params','artReview')
    foreach($key in $brief.PSObject.Properties.Name){if($key -notin $keys){throw "Unknown brief field: $key"}}
    if((Get-TripoField $brief 'schemaVersion') -ne 1){throw 'Brief schemaVersion must be 1.'}
    $id=Get-TripoField $brief 'id';$prompt=Get-TripoField $brief 'prompt';$model=Get-TripoField $brief 'model';$seed=Get-TripoField $brief 'seed'
    if($id -isnot [string] -or $id -cnotmatch '^[a-z][a-z0-9-]{2,63}$' -or $id -match '^(con|prn|aux|nul|com[1-9]|lpt[1-9])$'){throw 'Brief id must be a safe lowercase job name, 3..64 characters.'}
    if($prompt -isnot [string] -or [string]::IsNullOrWhiteSpace($prompt) -or $prompt.Length -gt 1024 -or $prompt.StartsWith('-') -or $prompt.Contains([char]0)){throw 'Prompt must contain 1..1024 characters and not begin with a dash.'}
    $models=@{'tripo-v3.1'='v3.1-20260211';'tripo-p1'='P1-20260311';'tripo-p2'='P2-20260801'}
    if($model -isnot [string] -or -not $models.ContainsKey($model)){throw 'Use the reviewed tripo-v3.1, tripo-p1 or tripo-p2 model alias.'}
    Assert-TripoInteger $seed 0 2147483647 'seed'
    $inputParams=Get-TripoField $brief 'params'
    if($null -eq $inputParams -or $inputParams -isnot [pscustomobject]){throw 'params must be a JSON object.'}
    $params=[ordered]@{model_seed=$seed;image_seed=$seed;texture_seed=$seed;texture=$true;pbr=$true}
    $boolKeys=@('texture','pbr','auto_size','quad','smart_low_poly','generate_parts','export_uv')
    $allowed=$boolKeys+@('model_seed','image_seed','texture_seed','face_limit','negative_prompt','texture_quality','geometry_quality','compress','export_orientation')
    foreach($property in $inputParams.PSObject.Properties){
        $key=$property.Name;$value=$property.Value
        if($key -notin $allowed){throw "Unsupported text-to-model parameter: $key"}
        if($key -in $boolKeys -and $value -isnot [bool]){throw "$key must be a JSON boolean."}
        if($key -in @('model_seed','image_seed','texture_seed')){Assert-TripoInteger $value 0 2147483647 $key}
        if($key -eq 'face_limit'){Assert-TripoInteger $value 50 2000000 $key}
        if($key -eq 'negative_prompt' -and ($value -isnot [string] -or $value.Length -gt 255 -or $value.Contains([char]0))){throw 'negative_prompt must be a string of at most 255 characters.'}
        if($key -eq 'texture_quality' -and $value -cnotin @('standard','detailed','extreme')){throw 'Invalid texture_quality.'}
        if($key -eq 'geometry_quality' -and $value -cnotin @('standard','detailed')){throw 'Invalid geometry_quality.'}
        if($key -eq 'compress' -and $value -cne 'geometry'){throw 'compress must be geometry.'}
        if($key -eq 'export_orientation' -and $value -cnotin @('+x','-x','+y','-y')){throw 'Invalid export_orientation.'}
        $params[$key]=$value
    }
    if($params.pbr -and -not $params.texture){throw 'pbr=true requires texture=true; make the authored intent explicit.'}
    if($params['generate_parts'] -and ($params.texture -or $params.pbr -or $params['quad'] -or $params['smart_low_poly'])){throw 'generate_parts requires texture, pbr, quad and smart_low_poly to be false.'}
    if($model -in @('tripo-p1','tripo-p2')){
        foreach($key in @('smart_low_poly','generate_parts','geometry_quality')){if($params.Contains($key)){throw "$model does not support $key."}}
        if($model -eq 'tripo-p1' -and $params.Contains('quad')){throw 'tripo-p1 does not support quad.'}
        $faceMax=if($model -eq 'tripo-p1'){20000}elseif($params['quad']){25000}else{50000}
        if($params.Contains('face_limit') -and $params.face_limit -gt $faceMax){throw 'face_limit exceeds the selected P-series model budget.'}
    } elseif($params['smart_low_poly'] -and $params.Contains('face_limit')){
        $faceMax=if($params['quad']){10000}else{20000}
        if($params.face_limit -lt 500 -or $params.face_limit -gt $faceMax){throw 'face_limit is incompatible with smart_low_poly.'}
    }
    return [pscustomobject]@{Brief=$brief;Path=$resolved;Hash=(Get-FileHash -LiteralPath $resolved -Algorithm SHA256).Hash;Params=$params;WireModel=$models[$model]}
}
function Get-TripoInstallation {
    $lockPath=Join-Path $workspace 'Config/tripo-lock.json'
    $lock=Get-Content -LiteralPath $lockPath -Raw -Encoding UTF8 | ConvertFrom-Json
    if($lock.schemaVersion -ne 1 -or $lock.cli.package -ne 'tripo-cli' -or $lock.cli.version -ne '0.6.1'){throw 'Unsupported Tripo CLI lock; this adapter is reviewed against 0.6.1.'}
    $shim=if($CliPath){[IO.Path]::GetFullPath($CliPath)}else{@(Get-Command 'tripo.cmd' -CommandType Application -ErrorAction Stop)[0].Source}
    Assert-NoReparsePoint $shim
    if(-not (Test-Path -LiteralPath $shim -PathType Leaf)){throw 'Tripo CLI shim is missing.'}
    $installRoot=Split-Path $shim -Parent
    $packagePath=Join-Path $installRoot 'node_modules/tripo-cli/package.json';Assert-NoReparsePoint $packagePath
    $package=Get-Content -LiteralPath $packagePath -Raw -Encoding UTF8 | ConvertFrom-Json
    if($package.name -ne 'tripo-cli' -or $package.version -ne $lock.cli.version){throw 'Installed Tripo CLI differs from Config/tripo-lock.json.'}
    $entry=Join-Path $installRoot 'node_modules/tripo-cli/dist/cli.js';Assert-NoReparsePoint $entry
    if(-not (Test-Path -LiteralPath $entry -PathType Leaf)){throw 'Official CLI entrypoint is missing.'}
    $node=Join-Path $installRoot 'node.exe'
    if(-not (Test-Path -LiteralPath $node -PathType Leaf)){$node=@(Get-Command 'node.exe' -CommandType Application -ErrorAction Stop)[0].Source}
    # Invoke the npm shim's JS target directly: native argv safely preserves quotes, &, and Unicode.
    $version=Invoke-WorkspaceProcess -FilePath $node -Arguments @($entry,'--version') -WorkingDirectory $workspace -TimeoutSeconds 15
    if($version.ExitCode -ne 0 -or $version.StdOut.Trim() -ne $lock.cli.version){throw 'Tripo CLI executable version check failed.'}
    return [pscustomobject]@{Node=$node;Entry=$entry;Version=$package.version}
}
function Save-TripoState {
    param([string]$Path,$State)
    $State.updatedUtc=[DateTime]::UtcNow.ToString('o')
    $temp=$Path+'.'+[Guid]::NewGuid().ToString('N')+'.tmp'
    Write-JsonFile $temp $State
    if(Test-Path -LiteralPath $Path){[IO.File]::Replace($temp,$Path,[NullString]::Value)}else{[IO.File]::Move($temp,$Path)}
}
function Read-TripoResult {
    param([string]$Output)
    # task get and no-wait generation return one JSON object. Also tolerate NDJSON progress.
    $last=$null
    foreach($line in ($Output -split '\r?\n')){
        if([string]::IsNullOrWhiteSpace($line)){continue}
        try{$parsed=$line | ConvertFrom-Json}catch{throw 'Tripo returned invalid JSON; no automatic retry was made.'}
        if((Get-TripoField $parsed 'task_id') -and -not (Get-TripoField $parsed 'event')){$last=$parsed}
    }
    if($null -eq $last){throw 'Tripo did not return a task result; inspect the remote task before retrying.'}
    $id=Get-TripoField $last 'task_id'
    if($id -isnot [string] -or $id -notmatch '^[A-Za-z0-9][A-Za-z0-9_-]{7,127}$'){throw 'Tripo returned an invalid task ID.'}
    return $last
}
function Invoke-TripoJson {
    param($Installation,[string[]]$Arguments)
    $result=Invoke-WorkspaceProcess -FilePath $Installation.Node -Arguments (@($Installation.Entry)+$Arguments+@('--json','--yes','--no-open')) -WorkingDirectory $workspace -TimeoutSeconds $TimeoutSeconds
    # Do not persist raw stderr/JSON: remote diagnostics can contain signed URLs or account details.
    $parsed=Read-TripoResult $result.StdOut
    return [pscustomobject]@{ExitCode=$result.ExitCode;Result=$parsed}
}
function Test-TripoArtifacts {
    param([string]$Job,$State)
    if(@($State.files).Count -eq 0){return $false}
    foreach($file in $State.files){
        $path=Resolve-ContainedPath $Job $file.path
        if(-not (Test-Path -LiteralPath $path -PathType Leaf)){return $false}
        if((Get-Item -LiteralPath $path).Length -ne $file.bytes -or (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ine $file.sha256){return $false}
    }
    return $true
}

if($TaskId -and $Mode -ne 'Resume'){throw '-TaskId is only for recovering an unknown submission with Resume.'}
if($Mode -eq 'Doctor'){
    $installation=Get-TripoInstallation
    [pscustomobject]@{status='ready';cliVersion=$installation.Version;localOnly=$true;authenticationChecked=$false;networkChecked=$false} | ConvertTo-Json
    return
}
$authored=Read-TripoBrief $BriefFile
$brief=$authored.Brief
$job=Resolve-ContainedPath $workspace ('Artifacts/tripo/'+$brief.id)
$statePath=Join-Path $job 'state.json'
$payload=[ordered]@{prompt=$brief.prompt;model=$authored.WireModel}
foreach($key in $authored.Params.Keys){$payload[$key]=$authored.Params[$key]}
if($Mode -eq 'Plan' -or ($Mode -eq 'Generate' -and -not $Execute)){
    [pscustomobject]@{status='planned';localOnly=$true;submissionCount=1;cliVersion='0.6.1';briefId=$brief.id;briefSha256=$authored.Hash;endpoint='/v3/generation/text-to-model';payload=$payload;stateFile=$statePath;requiresExecute=$true} | ConvertTo-Json -Depth 12
    return
}
$installation=Get-TripoInstallation
$null=New-Item -ItemType Directory -Path $job -Force
$guard=$null
try{
    try{$guard=[IO.File]::Open((Join-Path $job 'job.lock'),[IO.FileMode]::OpenOrCreate,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)}catch{throw 'This Tripo job is already in use; no task was submitted.'}
    $state=$null
    if(Test-Path -LiteralPath $statePath){
        Assert-NoReparsePoint $statePath
        $state=Get-Content -LiteralPath $statePath -Raw -Encoding UTF8 | ConvertFrom-Json
        if($state.schemaVersion -ne 1 -or $state.briefId -ne $brief.id -or $state.briefSha256 -ne $authored.Hash){throw 'Existing job belongs to different brief bytes. Use its original brief to resume, or explicitly author a new id.'}
    }
    if($Mode -eq 'Generate'){
        if($null -ne $state){throw 'This job already has a durable record. Use Resume; Generate never repeats a recorded submission.'}
        $state=[pscustomobject]@{schemaVersion=1;briefId=$brief.id;briefSha256=$authored.Hash;cliVersion=$installation.Version;status='planned';task_id=$null;serverStatus=$null;progress=$null;creditsConsumed=$null;createdUtc=[DateTime]::UtcNow.ToString('o');updatedUtc=$null;lastError=$null;files=@()}
        Write-JsonFile (Join-Path $job 'request.json') @{schemaVersion=1;briefId=$brief.id;briefSha256=$authored.Hash;endpoint='/v3/generation/text-to-model';payload=$payload}
        Save-TripoState $statePath $state
        $arguments=@('generate','text-to-model',$brief.prompt,'--model',$brief.model,'--name',$brief.id,'--no-wait','--no-download')
        foreach($key in $authored.Params.Keys){
            $value=$authored.Params[$key]
            $encoded=if($value -is [bool]){$value.ToString().ToLowerInvariant()}else{[string]$value}
            $arguments+=@('-p',($key+'='+$encoded))
        }
        # Write ahead of the network call. Even process termination cannot make Generate resubmit.
        $state.status='unknown';$state.lastError='Submission started; acceptance is not yet confirmed.'
        Save-TripoState $statePath $state
        try{
            $response=Invoke-TripoJson $installation $arguments
            $state.task_id=$response.Result.task_id;$state.status='submitted';$state.serverStatus=Get-TripoField $response.Result 'status'
            $state.lastError=if($response.ExitCode -eq 0){$null}else{'CLI returned a nonzero exit after returning a task ID; resume this task.'}
            Save-TripoState $statePath $state
        }catch{
            $state.lastError='Submission outcome is unknown. Do not resubmit. Verify the task in Tripo, then Resume with its TaskId.'
            Save-TripoState $statePath $state
            throw ('Submission outcome is unknown; durable state: '+$statePath)
        }
    }else{
        if($null -eq $state){throw 'No submitted job exists for this brief.'}
        if($TaskId){
            if($TaskId -notmatch '^[A-Za-z0-9][A-Za-z0-9_-]{7,127}$'){throw 'Invalid recovery TaskId.'}
            if($state.task_id -and $state.task_id -cne $TaskId){throw 'Recovery TaskId differs from the recorded task; no request made.'}
            if($state.status -ne 'unknown' -and -not $state.task_id){throw 'Only an unknown submission can adopt a verified TaskId.'}
            if(-not $state.task_id){$state.task_id=$TaskId;$state.status='submitted';$state.lastError=$null;Save-TripoState $statePath $state}
        }
        if(-not $state.task_id){throw 'Unknown submission has no task ID. Verify it in Tripo and use Resume -TaskId; never Generate again.'}
        if($state.status -eq 'complete'){
            if(Test-TripoArtifacts $job $state){$state | ConvertTo-Json -Depth 12;return}
            $state.status='submitted';$state.lastError='A downloaded artifact is missing or changed; restore it from the same task.'
            Save-TripoState $statePath $state
        }
        if($state.status -eq 'fail'){$state | ConvertTo-Json -Depth 12;return}
        $download=Resolve-ContainedPath $job 'downloads'
        try{
            $response=Invoke-TripoJson $installation @('task','get',$state.task_id,'--download','--out',$download)
            $detail=$response.Result
            if($detail.task_id -cne $state.task_id){throw 'Tripo returned a different task ID.'}
            if($response.ExitCode -ne 0){throw 'CLI query or download failed; resume the same task.'}
            $state.serverStatus=Get-TripoField $detail 'status';$state.progress=Get-TripoField $detail 'progress';$state.creditsConsumed=Get-TripoField $detail 'credits_consumed'
            if($state.serverStatus -eq 'success'){
                $fileRecords=@();$modelCount=0
                foreach($file in @(Get-TripoField $detail 'files')){
                    if(-not $file){continue}
                    $full=[IO.Path]::GetFullPath([string]$file)
                    if(-not $full.StartsWith($download+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'CLI download escaped its job directory.'}
                    Assert-NoReparsePoint $full
                    $item=Get-Item -LiteralPath $full
                    if($item.PSIsContainer -or $item.Length -le 0){throw 'A downloaded file is empty or invalid.'}
                    if($item.Extension.ToLowerInvariant() -in @('.glb','.gltf','.fbx','.obj','.stl','.3mf','.usdz','.zip','.ply')){$modelCount++}
                    $fileRecords+=@{path=$full.Substring($job.Length+1).Replace('\','/');bytes=$item.Length;sha256=(Get-FileHash -LiteralPath $full -Algorithm SHA256).Hash}
                }
                if($modelCount -eq 0){throw 'Successful task has no downloaded model; Resume to retry its download.'}
                $state.files=$fileRecords;$state.status='complete'
            }elseif($state.serverStatus -in @('failed','cancelled','canceled','banned','expired')){$state.status='fail'}
            else{$state.status='submitted'}
            $state.lastError=$null;Save-TripoState $statePath $state
        }catch{
            $state.lastError='Query or download did not complete. Task ID is retained; Resume the same job.'
            Save-TripoState $statePath $state
            throw ('Resume did not complete; task ID retained in '+$statePath)
        }
    }
    $state | ConvertTo-Json -Depth 12
}finally{if($null -ne $guard){$guard.Dispose()}}
