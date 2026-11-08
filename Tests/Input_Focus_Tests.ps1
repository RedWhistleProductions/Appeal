param([switch]$CheckRendering)
$ErrorActionPreference = 'Stop'
$Root = Split-Path $PSScriptRoot -Parent
$Start = New-Object System.Diagnostics.ProcessStartInfo
$Start.FileName = Join-Path $Root 'Appeal.exe'
$Start.Arguments = 'Test/Input_Focus'
$Start.WorkingDirectory = $Root
$Start.UseShellExecute = $false
$Start.CreateNoWindow = $true
$Start.RedirectStandardOutput = $true
$Start.RedirectStandardError = $true
$Start.EnvironmentVariables['APPEAL_RESOURCE_FOLDER'] = Join-Path $Root 'Resources'
$Process = New-Object System.Diagnostics.Process
$Process.StartInfo = $Start
try {
    [void]$Process.Start()
    $OutputTask = $Process.StandardOutput.ReadToEndAsync()
    $ErrorTask = $Process.StandardError.ReadToEndAsync()
    if (-not $Process.WaitForExit(10000)) {
        $Process.Kill()
        $Process.WaitForExit()
        throw 'Input focus script timed out.'
    }
    $Output = $OutputTask.GetAwaiter().GetResult().Replace("`r", '').TrimEnd("`n")
    $Errors = $ErrorTask.GetAwaiter().GetResult()
    $Expected = @('Game', '179.000000', 'GUI', '179.000000', '0.000000',
        '0', '0', '0', 'Game', '179.000000') -join "`n"
    if ($Process.ExitCode -ne 0 -or $Errors -ne '' -or $Output -cne $Expected) {
        throw "Input focus script failed:`n$Output`n$Errors"
    }
    Write-Output 'Input focus script passed (focus switching stops latched movement and does not replay it).'
} finally {
    $Process.Dispose()
}

if ($CheckRendering) {
    $InputDeps = Join-Path $Root 'Resources/Modules/Appeal_Module_Input/Appeal_SDL2_Input/.deps/windows-x64'
    $ImGui = Join-Path $Root 'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps/imgui'
    $RenderTest = Join-Path $Root 'Obj/SDL2_Focus_Render_Tests.exe'
    Push-Location $Root
    try {
        & g++ -std=c++17 -DSDL_MAIN_HANDLED "-I$ImGui" "-I$InputDeps/include/SDL2" (Join-Path $PSScriptRoot 'SDL2_Focus_Render.cpp') "-L$InputDeps/lib" -lSDL2 -o $RenderTest
        if ($LASTEXITCODE -ne 0) { throw 'Focus render test compilation failed.' }
        & $RenderTest
        if ($LASTEXITCODE -ne 0) { throw 'Focus render test failed.' }
    } finally {
        Pop-Location
    }
}
