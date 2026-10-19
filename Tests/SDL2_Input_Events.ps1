$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$plugin = Join-Path $root 'Resources/Modules/Appeal_Module_Input/Appeal_SDL2_Input'
$deps = Join-Path $plugin '.deps/windows-x64'
$test = Join-Path $env:TEMP ('Appeal_Input_Events_' + [guid]::NewGuid().ToString('N') + '.exe')
$previousPath = $env:PATH
Push-Location $root
try {
    $env:PATH = $root + ';' + $env:PATH
    foreach ($source in @('SDL2_Input_Events.cpp','SDL2_Input_Controls.cpp')) {
        & g++ -std=c++17 -Wall -DSDL_MAIN_HANDLED "-I$deps/include" "-I$deps/include/SDL2" (Join-Path $PSScriptRoot $source) (Join-Path $plugin 'src/main.cpp') "-L$deps/lib" -lSDL2 -o $test
        if ($LASTEXITCODE -ne 0) { throw "Input test build failed: $source" }
        & $test
        if ($LASTEXITCODE -ne 0) { throw "Input test failed: $source" }
    }
} finally {
    $env:PATH = $previousPath
    Pop-Location
    if (Test-Path -LiteralPath $test) { Remove-Item -LiteralPath $test }
}
