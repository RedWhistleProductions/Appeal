$ErrorActionPreference='Stop'
$root=(Get-Location).Path
$projects=Get-ChildItem Resources/Modules -Directory | ForEach-Object { $_; Get-ChildItem -LiteralPath $_.FullName -Directory | Where-Object { Test-Path (Join-Path $_.FullName 'Makefile') } }
foreach($project in $projects) {
    $output=& make -C $project.FullName -f Makefile -j4 2>&1
    if($LASTEXITCODE -ne 0){ $output; throw "Build failed: $($project.Name)" }
    $binary=Join-Path $project.FullName ($project.Name + '.dll')
    if(-not (Test-Path -LiteralPath $binary)){ throw "Expected runtime DLL missing: $binary" }
    if($project.Name -like 'Appeal_Module_*') {
        $symbols=& nm $binary
        if(-not ($symbols -match '\bT Shutdown$')){ throw "Runtime module lacks Shutdown: $binary" }
    }
    Write-Output "Built $($project.Name)"
}
