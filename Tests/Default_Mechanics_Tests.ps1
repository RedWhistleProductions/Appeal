$ErrorActionPreference = 'Stop'
$Root = Split-Path $PSScriptRoot -Parent
$Start = New-Object System.Diagnostics.ProcessStartInfo
$Start.FileName = Join-Path $Root 'Appeal.exe'
$Start.Arguments = 'Test/Default_Mechanics'
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
    if (-not $Process.WaitForExit(15000)) {
        $Process.Kill()
        $Process.WaitForExit()
        throw 'Default Mechanics integration timed out.'
    }
    $Output = $OutputTask.GetAwaiter().GetResult().Replace("`r", '').TrimEnd("`n")
    $Errors = $ErrorTask.GetAwaiter().GetResult()
    $Expected = @('10.000000', '-5.000000', '0.000000', '20.000000', '1.000000',
        '2.500000', '2.000000', '2.500000', 'NPC', '4.000000', '3.000000',
        'Jump', 'Ground', '', 'Debug', 'Set_Paths', 'Enter', 'Exit', '2.000000',
        '2D_Side_Scrolling', '2D_Top_Down', '4.000000', '2.828427', '2.828427',
        '-2.000000', '6.000000', 'Jump', '3D', '4.000000', '3.000000', '0.750000',
        '4.000000', '6.000000', '-2.000000', 'Ground', 'Top_Down') -join "`n"
    if ($Process.ExitCode -ne 0 -or $Errors -ne '' -or $Output -cne $Expected) {
        throw "Default Mechanics integration failed ($($Process.ExitCode)):`n$Output`n$Errors"
    }
    Write-Output 'Default Mechanics integration passed (all three orientations, selected actions, analog input, globals, and simultaneous plugins).'
} finally {
    $Process.Dispose()
}
