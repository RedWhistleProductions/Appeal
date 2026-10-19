param([switch]$CheckRendering)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
function Run-Appeal([string]$script) {
    $start = New-Object System.Diagnostics.ProcessStartInfo
    $start.FileName = Join-Path $root 'Appeal.exe'
    $start.Arguments = $script
    $start.WorkingDirectory = $root
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    $start.EnvironmentVariables['APPEAL_RESOURCE_FOLDER'] = Join-Path $root 'Resources'
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $start
    try {
        [void]$process.Start()
        $stdout = $process.StandardOutput.ReadToEndAsync()
        $stderr = $process.StandardError.ReadToEndAsync()
        if (-not $process.WaitForExit(20000)) {
            $process.Kill()
            $process.WaitForExit()
            throw "Timed out: $script"
        }
        $output = $stdout.GetAwaiter().GetResult().Replace("`r", '')
        $errors = $stderr.GetAwaiter().GetResult()
        if ($process.ExitCode -ne 0 -or $errors -or $output -match '(?im)(error:|failed|not found|did not load)') {
            throw "$script failed:`n$output`n$errors"
        }
        return $output
    } finally { $process.Dispose() }
}
$unit = Join-Path $env:TEMP ('Appeal_Animation_' + [guid]::NewGuid().ToString('N') + '.exe')
try {
    & g++ -std=c++17 -Wall -Wextra (Join-Path $PSScriptRoot 'Graphics_Animation_Tests.cpp') -o $unit
    if ($LASTEXITCODE -ne 0) { throw 'Animation unit test compilation failed.' }
    & $unit
    if ($LASTEXITCODE -ne 0) { throw 'Animation unit tests failed.' }
} finally { if (Test-Path -LiteralPath $unit) { Remove-Item -LiteralPath $unit } }
$test = Run-Appeal 'Test/Graphics_Test_Plugin'
foreach ($line in @(
    'Load_Animation Adam Standing Examples/Characters/Adam_Movement.png 64 64 0 8',
    'Set_Animation Adam Standing', 'Set_Animation_FPS Adam Standing 7',
    'Set_Animation_Mode Adam Standing Hold', 'Rotate Adam 10 20 30',
    'Paste Adam 176 204 0', 'Animation_Frame Adam', 'Current_Animation Adam',
    'Reset_Animation Adam', 'GRAPHICS_TEST_PLUGIN_PASSED'
)) {
    if ($test -notmatch ('(?m)^' + [regex]::Escape($line) + '$')) { throw "Missing Test plugin output: $line" }
}
Write-Output 'GRAPHICS_TEST_PLUGIN_PASSED'
$sdl = Run-Appeal 'Test/Graphics_Animations'
foreach ($expected in @(
    "1`n1`n1`nINITIAL`n0`n0", "ADVANCED`n1`n2", "WALKING`nWalking`n0",
    "RESET`nStanding`n0", "INVALID`n0`n0`nStanding", "HELD`n7",
    "RETURNED`nStanding`n0", "FREED`n-1", "RELOADED`n1", 'GRAPHICS_ANIMATION_SDL_PASSED'
)) {
    if (-not $sdl.Contains($expected)) { throw "Missing SDL result: $expected`n$sdl" }
}
Write-Output 'GRAPHICS_ANIMATION_SDL_PASSED'
if ($CheckRendering) {
    $render = Join-Path $env:TEMP ('Appeal_Animation_Render_' + [guid]::NewGuid().ToString('N') + '.exe')
    Push-Location $root
    try {
        & g++ -std=c++17 (Join-Path $PSScriptRoot 'Graphics_Animation_Render_Tests.cpp') -lopengl32 -o $render
        if ($LASTEXITCODE -ne 0) { throw 'Render test compilation failed.' }
        & $render
        if ($LASTEXITCODE -ne 0) { throw 'Framebuffer regression failed.' }
    } finally {
        Pop-Location
        if (Test-Path -LiteralPath $render) { Remove-Item -LiteralPath $render }
    }
}
