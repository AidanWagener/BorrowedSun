<# Offline adapter regression tests. The fake CLI has no networking or credentials. #>
[CmdletBinding()]
param()
. "$PSScriptRoot/../Common.ps1"
$toolsRoot=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $toolsRoot -Parent
$context=Get-WorkspaceContext
$run=New-RunDirectory $context 'tripo-tests'
$fixture=Join-Path $run 'Workspace With Spaces'
$fixtureTools=Join-Path $fixture 'Tools'
$null=New-Item -ItemType Directory -Path $fixtureTools
$null=New-Item -ItemType Directory -Path (Join-Path $fixture 'Config')
foreach($name in @('Tripo.ps1','Common.ps1')){Copy-Item -LiteralPath (Join-Path $toolsRoot $name) -Destination (Join-Path $fixtureTools $name)}
Write-JsonFile (Join-Path $fixture 'Config/tripo-lock.json') @{schemaVersion=1;cli=@{package='tripo-cli';version='0.6.1'}}
$mock=Join-Path $run 'Mock CLI With Spaces'
$module=Join-Path $mock 'node_modules/tripo-cli'
$dist=Join-Path $module 'dist'
$null=New-Item -ItemType Directory -Path $dist
$shim=Join-Path $mock 'tripo.cmd'
'@echo off' | Set-Content -LiteralPath $shim -Encoding ASCII
Write-JsonFile (Join-Path $module 'package.json') @{name='tripo-cli';version='0.6.1';type='module'}
$controlPath=Join-Path $dist 'control.json'
$callsPath=Join-Path $dist 'calls.jsonl'
@'
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const root=path.dirname(fileURLToPath(import.meta.url));
const args=process.argv.slice(2);
if(args[0]==='--version'){console.log('0.6.1');process.exit(0);}
if(args[0]==='fixture-file-link'){fs.symlinkSync(args[2],args[1],'file');process.exit(0);}
const control=JSON.parse(fs.readFileSync(path.join(root,'control.json'),'utf8'));
fs.appendFileSync(path.join(root,'calls.jsonl'),JSON.stringify(args)+'\n');
if(args[0]==='generate'){
  if(control.submit==='unknown'){console.error('sensitive-diagnostic-sentinel');process.exit(8);}
  if(control.submit==='invalid'){console.log('not json');process.exit(0);}
  console.log(JSON.stringify({task_id:'task_fixture_001',status:'queued'}));
}else if(args[0]==='task'&&args[1]==='get'){
  const detail={task_id:args[2],status:control.status,progress:42,credits_consumed:35};
  if(control.status==='success'){
    const out=args[args.indexOf('--out')+1];fs.mkdirSync(out,{recursive:true});
    detail.output={model_url:'https://example.invalid/model.glb?secret=signed-sentinel'};
    detail.files=[];
    if(!control.empty){const file=path.join(out,'model.glb');fs.writeFileSync(file,'OFFLINE TEST FIXTURE, NOT A 3D MODEL');detail.files.push(file);}
    fs.writeFileSync(path.join(out,'task.json'),JSON.stringify({task_id:detail.task_id}));
  }
  console.log(JSON.stringify(detail));
}else{console.error('Unexpected mock command');process.exit(9);}
'@ | Set-Content -LiteralPath (Join-Path $dist 'cli.js') -Encoding UTF8
$adapter=Join-Path $fixtureTools 'Tripo.ps1'
$briefPath=Join-Path $fixture 'brief.json'
$prompt='Handmade "sun" lamp & repaired brass, 100% useful; no pipes | no redirect >. Unicode: caf'+[char]233+'.'
$brief=@{schemaVersion=1;id='fixture-lamp-v1';prompt=$prompt;model='tripo-v3.1';seed=41001;params=@{face_limit=30000;texture=$true;pbr=$true;texture_quality='detailed';geometry_quality='detailed';auto_size=$true}}
Write-JsonFile $briefPath $brief
Write-JsonFile $controlPath @{submit='ok';status='running'}
$passed=0
$skipped=@()
function Assert-True {param([bool]$Value,[string]$Message);if(-not $Value){throw $Message};$script:passed++}
function Expect-Failure {
    param([scriptblock]$Action,[string]$Message)
    $failure=$null
    try{& $Action | Out-Null}catch{$failure=$_.Exception.Message}
    if(-not $failure){throw $Message}
    if($failure.Contains('sensitive-diagnostic-sentinel')){throw 'CLI stderr leaked to the caller.'}
    $script:passed++
}
function Read-Calls {if(Test-Path -LiteralPath $callsPath){Get-Content -LiteralPath $callsPath -Encoding UTF8 | ForEach-Object {[pscustomobject]@{arguments=($_ | ConvertFrom-Json)}}}}
function Count-Submits {return @((Read-Calls) | Where-Object {$_.arguments[0] -eq 'generate'}).Count}
function Read-State {Get-Content -LiteralPath (Join-Path $fixture ('Artifacts/tripo/'+$brief.id+'/state.json')) -Raw -Encoding UTF8 | ConvertFrom-Json}
function New-FixtureFileLink {
    param([string]$Link,[string]$Target,[string]$Case)
    $node=@(Get-Command 'node.exe' -CommandType Application -ErrorAction Stop)[0].Source
    $result=Invoke-WorkspaceProcess -FilePath $node -Arguments @((Join-Path $dist 'cli.js'),'fixture-file-link',$Link,$Target) -WorkingDirectory $fixture -TimeoutSeconds 15
    if($result.ExitCode -ne 0){
        if($result.StdErr -match '\b(EPERM|EACCES)\b'){
            $script:skipped+=@{Case=$Case;Reason='Windows denied file-symbolic-link creation; no file-link assertions ran.'}
            return $false
        }
        throw 'Could not create the file-symlink regression fixture.'
    }
    return $true
}

$plan=& $adapter -Mode Plan -BriefFile $briefPath | ConvertFrom-Json
Assert-True ($plan.localOnly -and $plan.payload.model -eq 'v3.1-20260211' -and $plan.payload.model_seed -eq 41001 -and $plan.payload.texture_seed -eq 41001) 'Plan did not preserve endpoint/model/seeds.'
Assert-True (-not (Test-Path -LiteralPath (Join-Path $fixture 'Artifacts')) -and -not (Test-Path -LiteralPath $callsPath)) 'Plan performed a side effect or invoked the CLI.'
$dry=& $adapter -Mode Generate -BriefFile $briefPath | ConvertFrom-Json
Assert-True ($dry.status -eq 'planned' -and -not (Test-Path -LiteralPath $callsPath)) 'Generate without Execute was not a dry plan.'
$doctor=& $adapter -Mode Doctor -CliPath $shim | ConvertFrom-Json
Assert-True ($doctor.cliVersion -eq '0.6.1' -and -not $doctor.authenticationChecked -and -not (Test-Path -LiteralPath $callsPath)) 'Doctor queried an account or failed its local version check.'
$submitted=& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute | ConvertFrom-Json
Assert-True ($submitted.status -eq 'submitted' -and $submitted.task_id -eq 'task_fixture_001' -and (Count-Submits) -eq 1) 'Actual task ID was not durably saved after one submission.'
$call=@(Read-Calls)[0].arguments
Assert-True ($call[2] -ceq $prompt -and $call -contains '--no-wait' -and $call -contains '--no-download') 'Native argv changed prompt bytes or lost submission-only flags.'
Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'Duplicate generation was accepted.'
Assert-True ((Count-Submits) -eq 1) 'Duplicate guard still submitted another task.'
$running=& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim | ConvertFrom-Json
Assert-True ($running.status -eq 'submitted' -and $running.task_id -eq 'task_fixture_001') 'Running task lost its task ID.'
Expect-Failure {& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim -TaskId task_wrong_002} 'Resume replaced a known task ID.'
$brief.prompt='Changed authoring intent';Write-JsonFile $briefPath $brief
Expect-Failure {& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim} 'Changed brief bytes were accepted for an existing job.'
$brief.prompt=$prompt;Write-JsonFile $briefPath $brief
Write-JsonFile $controlPath @{submit='ok';status='success';empty=$true}
Expect-Failure {& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim} 'A success response without downloaded geometry was marked complete.'
Assert-True ((Read-State).status -eq 'submitted') 'Download failure discarded the resumable task state.'
Write-JsonFile $controlPath @{submit='ok';status='success'}
$complete=& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim | ConvertFrom-Json
Assert-True ($complete.status -eq 'complete' -and $complete.files.Count -eq 1 -and $complete.files[0].sha256.Length -eq 64 -and $complete.creditsConsumed -eq 35) 'Completion did not verify and hash downloaded artifacts.'
$queries=@(Read-Calls | Where-Object {$_.arguments[0] -eq 'task'})
Assert-True (@($queries | Where-Object {$_.arguments[2] -ne 'task_fixture_001'}).Count -eq 0) 'Resume queried a different task ID.'
$saved=Get-Content -LiteralPath (Join-Path $fixture ('Artifacts/tripo/'+$brief.id+'/state.json')) -Raw -Encoding UTF8
Assert-True (-not $saved.Contains('signed-sentinel')) 'Signed download URLs leaked into the durable state.'
$downloads=Join-Path $fixture ('Artifacts/tripo/'+$brief.id+'/downloads')
$modelPath=Resolve-ContainedPath $downloads 'model.glb'
[IO.File]::WriteAllText($modelPath,'tampered offline fixture')
$restored=& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim | ConvertFrom-Json
Assert-True ($restored.status -eq 'complete' -and $restored.files[0].sha256 -eq $complete.files[0].sha256) 'Resume failed to restore a corrupted download from its original task.'
Remove-Item -LiteralPath $modelPath
$restored=& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim | ConvertFrom-Json
Assert-True ($restored.status -eq 'complete' -and (Test-Path -LiteralPath $modelPath)) 'Resume failed to restore a missing download.'
Assert-True ((Count-Submits) -eq 1) 'Restoring downloaded artifacts submitted new geometry.'

foreach($leaf in @('model.glb','task.json')){
    $brief.id=if($leaf -eq 'model.glb'){'linked-model-v1'}else{'linked-metadata-v1'}
    Write-JsonFile $briefPath $brief
    & $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute | Out-Null
    $downloads=Join-Path $fixture ('Artifacts/tripo/'+$brief.id+'/downloads')
    $null=New-Item -ItemType Directory -Path $downloads
    $external=Join-Path $run ($brief.id+'-external-sentinel')
    [IO.File]::WriteAllText($external,'Unrelated file must not change.')
    $externalHash=(Get-FileHash -LiteralPath $external -Algorithm SHA256).Hash
    $link=Join-Path $downloads $leaf
    if(-not (New-FixtureFileLink $link $external $brief.id)){continue}
    $callsBefore=@(Read-Calls).Count
    try{
        Expect-Failure {& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim} 'Resume accepted an existing file symlink before download.'
        Assert-True (@(Read-Calls).Count -eq $callsBefore) 'A linked download destination reached the CLI before rejection.'
        Assert-True ((Get-FileHash -LiteralPath $external -Algorithm SHA256).Hash -eq $externalHash) 'Resume overwrote an unrelated file through a download symlink.'
        Assert-True ((Read-State).status -eq 'submitted' -and (Read-State).task_id -eq 'task_fixture_001') 'Rejecting a download symlink lost the resumable task ID.'
    }finally{Remove-Item -LiteralPath $link -Force}
    $repaired=& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim | ConvertFrom-Json
    Assert-True ($repaired.status -eq 'complete' -and $repaired.task_id -eq 'task_fixture_001') 'Removing the download symlink did not allow the original task to resume.'
}

$brief.id='nested-junction-v1';Write-JsonFile $briefPath $brief
& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute | Out-Null
$downloads=Join-Path $fixture ('Artifacts/tripo/'+$brief.id+'/downloads')
$nested=Join-Path $downloads 'one/two';$null=New-Item -ItemType Directory -Path $nested
$junction=Join-Path $nested 'cycle'
$null=New-Item -ItemType Junction -Path $junction -Value $downloads
$callsBefore=@(Read-Calls).Count
try{
    Expect-Failure {& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim} 'Resume accepted a nested directory junction.'
    Assert-True (@(Read-Calls).Count -eq $callsBefore) 'A nested junction reached the CLI before rejection.'
    Assert-True ((Read-State).task_id -eq 'task_fixture_001') 'Rejecting a nested junction lost the resumable task ID.'
}finally{[IO.Directory]::Delete($junction)}

$brief.id='linked-request-v1';Write-JsonFile $briefPath $brief
$job=Join-Path $fixture ('Artifacts/tripo/'+$brief.id);$null=New-Item -ItemType Directory -Path $job
$external=Join-Path $run 'request-external-sentinel';[IO.File]::WriteAllText($external,'Unrelated request sentinel.')
$externalHash=(Get-FileHash -LiteralPath $external -Algorithm SHA256).Hash
$link=Join-Path $job 'request.json'
if(New-FixtureFileLink $link $external $brief.id){
    $callsBefore=@(Read-Calls).Count
    try{
        Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'Generate accepted a linked request record.'
        Assert-True (@(Read-Calls).Count -eq $callsBefore) 'A linked request record reached the CLI before rejection.'
        Assert-True ((Get-FileHash -LiteralPath $external -Algorithm SHA256).Hash -eq $externalHash) 'Generate overwrote an unrelated file through request.json.'
        Assert-True (-not (Test-Path -LiteralPath (Join-Path $job 'state.json'))) 'Rejected request path created a submission record.'
    }finally{Remove-Item -LiteralPath $link -Force}
}

$brief.id='unknown-lamp-v1';Write-JsonFile $briefPath $brief
Write-JsonFile $controlPath @{submit='unknown';status='running'}
$before=Count-Submits
Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'Ambiguous submission was accepted as confirmed.'
Assert-True ((Read-State).status -eq 'unknown' -and (Count-Submits) -eq ($before+1)) 'Ambiguous submission was not durably marked unknown.'
Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'Ambiguous submission was automatically repeated.'
Expect-Failure {& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim} 'Unknown task was resumed without a verified ID.'
Assert-True ((Count-Submits) -eq ($before+1)) 'Unknown recovery path submitted another generation.'
Write-JsonFile $controlPath @{submit='ok';status='success'}
$recovered=& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim -TaskId task_verified_999 | ConvertFrom-Json
Assert-True ($recovered.status -eq 'complete' -and $recovered.task_id -eq 'task_verified_999') 'Explicit verified task ID recovery failed.'

$brief.id='failed-lamp-v1';Write-JsonFile $briefPath $brief
Write-JsonFile $controlPath @{submit='ok';status='failed'}
& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute | Out-Null
$failed=& $adapter -Mode Resume -BriefFile $briefPath -CliPath $shim | ConvertFrom-Json
Assert-True ($failed.status -eq 'fail' -and $failed.task_id -eq 'task_fixture_001') 'A terminal server failure was not retained as failed.'
Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'A terminal failed job was silently regenerated.'
$brief.id='malformed-lamp-v1';Write-JsonFile $briefPath $brief
Write-JsonFile $controlPath @{submit='invalid';status='running'}
Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'Malformed submission JSON was accepted.'
Assert-True ((Read-State).status -eq 'unknown') 'Malformed submission JSON lost the unknown transaction state.'

$brief.id='locked-lamp-v1';Write-JsonFile $briefPath $brief
$job=Join-Path $fixture ('Artifacts/tripo/'+$brief.id);$null=New-Item -ItemType Directory -Path $job
$guard=[IO.File]::Open((Join-Path $job 'job.lock'),[IO.FileMode]::OpenOrCreate,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
try{Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'Concurrent job lock was ignored.'}finally{$guard.Dispose()}
$brief.id='../escape';Write-JsonFile $briefPath $brief
Expect-Failure {& $adapter -Mode Plan -BriefFile $briefPath} 'Path traversal job ID was accepted.'
$brief.id='invalid-quality-v1';$brief.params.texture_quality='Detailed';Write-JsonFile $briefPath $brief
Expect-Failure {& $adapter -Mode Plan -BriefFile $briefPath} 'Invalid quality casing passed local validation despite the CLI case-sensitive enum.'
$brief.params.texture_quality='detailed'
$brief.id='version-lamp-v1';Write-JsonFile $briefPath $brief
Write-JsonFile (Join-Path $module 'package.json') @{name='tripo-cli';version='0.6.2';type='module'}
Expect-Failure {& $adapter -Mode Generate -BriefFile $briefPath -CliPath $shim -Execute} 'Unreviewed CLI version was accepted.'
Assert-True (-not (Test-Path -LiteralPath (Join-Path $fixture ('Artifacts/tripo/'+$brief.id+'/state.json')))) 'Version mismatch created a submission record.'
$summary=@{Passed=$passed;Failed=0;Skipped=$skipped.Count;SkippedCases=$skipped;Scope='Offline Tripo adapter tests with fake CLI; no network, credentials, billing or real model generation';Fixtures=$run}
Write-JsonFile (Join-Path $run 'tripo-test-result.json') $summary
$summary | ConvertTo-Json
