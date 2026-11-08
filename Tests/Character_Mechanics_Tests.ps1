$ErrorActionPreference = 'Stop'
$Root = Split-Path $PSScriptRoot -Parent
$Start = New-Object System.Diagnostics.ProcessStartInfo
$Start.FileName = Join-Path $Root 'Appeal.exe'
$Start.Arguments = 'Test/Character_Mechanics'
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
        throw 'Character Mechanics script timed out.'
    }
    $Output = $OutputTask.GetAwaiter().GetResult().Replace("`r", '').TrimEnd("`n")
    $Errors = $ErrorTask.GetAwaiter().GetResult()
    $Expected = @('179.000000', '432.000000', '185.000000', 'Run',
        '186.500000', 'Duck_Walk', 'Duck_Stand', '9.750000', 'Jump',
        '430.500000', '-90.000000', '2D_Side_Scrolling') -join "`n"
    if ($Process.ExitCode -ne 0 -or $Errors -ne '' -or $Output -cne $Expected) {
        throw "Character Mechanics script failed:`n$Output`n$Errors"
    }
    Write-Output 'Character Mechanics scripts passed (walking, running, ducking, jumping, analog input, and entity isolation).'
} finally {
    $Process.Dispose()
}
