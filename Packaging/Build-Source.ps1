param(
    [string]$Version = (Get-Content "$PSScriptRoot\..\VERSION" -Raw).Trim()
)

$ErrorActionPreference = "Stop"

$Root = Resolve-Path "$PSScriptRoot\.."
$Dist = Join-Path $Root "Dist"
$Stage = Join-Path $Dist "Appeal-$Version-source"
$Archive = Join-Path $Dist "Appeal-$Version-source.zip"

if(Test-Path $Stage) {
    Remove-Item $Stage -Recurse -Force
}

New-Item -ItemType Directory -Path $Stage | Out-Null

$ExcludeDirs = @(".git", ".agents", ".vscode", "Dist", "Obj")
$ExcludeFiles = @("Appeal", "Appeal.exe")

Get-ChildItem $Root -Force | ForEach-Object {
    if($_.PSIsContainer) {
        if($ExcludeDirs -notcontains $_.Name) {
            Copy-Item $_.FullName (Join-Path $Stage $_.Name) -Recurse
        }
    }
    else {
        if($ExcludeFiles -notcontains $_.Name) {
            Copy-Item $_.FullName $Stage
        }
    }
}

if(Test-Path $Archive) {
    Remove-Item $Archive -Force
}

Compress-Archive -Path (Join-Path $Stage "*") -DestinationPath $Archive
Write-Host "Created $Archive"
