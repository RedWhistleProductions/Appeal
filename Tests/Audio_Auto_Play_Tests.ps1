# Exercise the real SDL2 plugin without any script-side Audio Update calls.
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$id = 'Auto_Play_Smoke_' + [guid]::NewGuid().ToString('N')
$scriptPath = Join-Path $root "Resources/Appeal/Test/$id.Appeal"
$shortPath = Join-Path $root "Resources/Audio/Music/${id}_short.wav"
$longPath = Join-Path $root "Resources/Audio/Music/${id}_long.wav"

function Write-SilentWave($path, $seconds) {
    $samples = [int](44100 * $seconds)
    $writer = [IO.BinaryWriter]::new([IO.File]::Create($path))
    try {
        $writer.Write([Text.Encoding]::ASCII.GetBytes('RIFF'))
        $writer.Write([int](36 + $samples * 2))
        $writer.Write([Text.Encoding]::ASCII.GetBytes('WAVEfmt '))
        $writer.Write([int]16)
        $writer.Write([int16]1)
        $writer.Write([int16]1)
        $writer.Write([int]44100)
        $writer.Write([int]88200)
        $writer.Write([int16]2)
        $writer.Write([int16]16)
        $writer.Write([Text.Encoding]::ASCII.GetBytes('data'))
        $writer.Write([int]($samples * 2))
        $writer.Write([byte[]]::new($samples * 2))
    } finally { $writer.Dispose() }
}

try {
    Write-SilentWave $shortPath 0.2
    Write-SilentWave $longPath 5
    $cases = @(
        @{ Name = 'automatic advancement'; Commands = "Audio Auto_Play_Next 1`nAudio Play_Track 1`nDelay 1"; Song = 'Second'; Playing = $true },
        @{ Name = 'disabled advancement'; Commands = "Audio Auto_Play_Next 1`nAudio Play_Track 1`nAudio Auto_Play_Next 0`nDelay 1"; Song = 'First'; Playing = $false },
        @{ Name = 'stop cancels advancement'; Commands = "Audio Auto_Play_Next 1`nAudio Play_Track 1`nAudio Stop`nDelay 1"; Song = 'First'; Playing = $false },
        @{ Name = 'looping keeps current song'; Commands = "Audio Auto_Play_Next 1`nAudio Play_Track 1`nAudio Set_Current_Loop 1`nDelay 1"; Song = 'First'; Playing = $true },
        @{ Name = 'manual next does not skip'; Commands = "Audio Auto_Play_Next 1`nAudio Play_Track 1`nAudio Next`nDelay 1"; Song = 'Second'; Playing = $true }
    )
    foreach ($case in $cases) {
        $script = @"
Add_Dynamic_Module Audio Appeal_SDL2_Audio
Audio Debug On
Audio Load_Song First "${id}_short.wav"
Audio Load_Song Second "${id}_long.wav"
$($case.Commands)
Audio Current_Song
If ( Audio Is_Playing ) == 1 { Print PlayingNow }
Done
"@
        [IO.File]::WriteAllText($scriptPath, $script)
        $start = [Diagnostics.ProcessStartInfo]::new()
        $start.FileName = Join-Path $root 'Appeal.exe'
        $start.Arguments = "Test/$id"
        $start.WorkingDirectory = $root
        $start.UseShellExecute = $false
        $start.CreateNoWindow = $true
        $start.RedirectStandardOutput = $true
        $start.RedirectStandardError = $true
        $start.EnvironmentVariables['APPEAL_RESOURCE_FOLDER'] = Join-Path $root 'Resources'
        $process = [Diagnostics.Process]::new()
        $process.StartInfo = $start
        try {
            [void]$process.Start()
            $stdout = $process.StandardOutput.ReadToEndAsync()
            $stderr = $process.StandardError.ReadToEndAsync()
            if (!$process.WaitForExit(10000)) {
                $process.Kill()
                $process.WaitForExit()
                throw "Audio test timed out: $($case.Name)"
            }
            $output = $stdout.GetAwaiter().GetResult()
            $errors = $stderr.GetAwaiter().GetResult()
            if ($process.ExitCode -ne 0 -or $errors -ne '' -or $output -match 'did not|not found|Error|Warning' -or
                $output -notmatch "(?m)^$($case.Song)\r?$" -or
                ($output.Contains('PlayingNow') -ne $case.Playing)) {
                throw "Audio test failed: $($case.Name)`n$output`n$errors"
            }
            Write-Output "Passed: $($case.Name)"
        } finally { $process.Dispose() }
    }
} finally {
    Remove-Item -LiteralPath $scriptPath,$shortPath,$longPath -ErrorAction SilentlyContinue
}
