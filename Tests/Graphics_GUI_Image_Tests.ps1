$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$test = Join-Path $env:TEMP ('Appeal_GUI_Image_' + [guid]::NewGuid().ToString('N') + '.exe')
Push-Location $root
try {
    & g++ -std=c++17 Tests/Graphics_GUI_Image_Tests.cpp -lopengl32 -o $test
    if ($LASTEXITCODE -ne 0) { throw 'GUI image test build failed' }
    & $test
    if ($LASTEXITCODE -ne 0) { throw 'GUI image test failed' }
} finally {
    Pop-Location
    if (Test-Path -LiteralPath $test) { Remove-Item -LiteralPath $test }
}
