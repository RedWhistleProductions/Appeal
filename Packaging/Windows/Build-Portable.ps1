param(
    [string]$Version = (Get-Content "$PSScriptRoot\..\..\VERSION" -Raw).Trim()
)

$ErrorActionPreference = "Stop"

$Root = Resolve-Path "$PSScriptRoot\..\.."
$Dist = Join-Path $Root "Dist"
$Stage = Join-Path $Dist "Appeal-$Version-windows-x64"
$Archive = Join-Path $Dist "Appeal-$Version-windows-x64.zip"

Set-Location $Root

make -f Makefile clean
make -f Makefile

if(Test-Path $Stage) {
    Remove-Item $Stage -Recurse -Force
}

New-Item -ItemType Directory -Path $Stage | Out-Null
Copy-Item "Appeal.exe" $Stage
Copy-Item "Resources" $Stage -Recurse
Copy-Item "README.md" $Stage
Copy-Item "LICENSE" $Stage
Copy-Item "VERSION" $Stage

if(Test-Path $Archive) {
    Remove-Item $Archive -Force
}

Compress-Archive -Path (Join-Path $Stage "*") -DestinationPath $Archive
Write-Host "Created $Archive"
