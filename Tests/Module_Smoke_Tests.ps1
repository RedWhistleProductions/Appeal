# Run the real Main script with the bundled Test plugins. Unlike the small
# preprocessor tests, this catches missing DLLs and script/module API drift.
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$start = New-Object System.Diagnostics.ProcessStartInfo
$start.FileName = Join-Path $root 'Appeal.exe'
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
    if (-not $process.WaitForExit(15000)) {
        $process.Kill()
        $process.WaitForExit()
        throw 'Module tests hung or crashed before completing.'
    }
    $output = $stdout.GetAwaiter().GetResult()
    $errors = $stderr.GetAwaiter().GetResult()
    if ($process.ExitCode -ne 0 -or $errors -ne '' -or $output -match '(?im)\b(error|warning):') {
        throw "Module tests failed:`n$output`n$errors"
    }
    # Check formerly failing calls and the end of each script. Exit code alone
    # is insufficient: Appeal also stops after too many unknown commands.
    foreach ($line in @(
        'Load_Play_List File_Name', 'Auto_Play_Next 1',
        'GUI_New_Game_Project_Button Button2 New Game Name_Input Picker1',
        'GUI_File_Picker_Selected_Path Picker1', 'GUI_Music_Picker_Selected_Title Music1',
        'Wait_Until_Close Main', 'Mouse_X',
        'Set_Controller_Axis_Range 0 Left_Stick -100 100 On_Move',
        'Configure HTML 172.0.0.1 80', 'Send Chat, Hello World',
        'Receive Chat', 'Download Appeal, Test_File.txt'
    )) {
        if ($output -notmatch ('(?m)^' + [regex]::Escape($line) + '\r?$')) {
            throw "Missing test output: $line`n$output"
        }
    }
    Write-Output 'Audio, Graphics, Input, and Network script smoke tests passed.'
} finally {
    $process.Dispose()
}
