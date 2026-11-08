$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$names = 'AI', 'Storage', 'Package', 'World', 'Files', 'Mechanics', 'Physics'
$expected = New-Object System.Collections.Generic.List[string]
foreach ($name in $names) {
    $called = New-Object System.Collections.Generic.List[string]
    # These modules initialize the default slot's Debug and Set_Paths hooks.
    if ($name -in 'AI', 'Mechanics', 'Physics') {
        $expected.Add('Debug')
        $expected.Add('Set_Paths')
    }
    foreach ($line in Get-Content "$root/Resources/Appeal/Test/$name.Appeal") {
        if ($line -match ('^' + $name + ' (\w+)')) {
            $function = $Matches[1]
            if ($name -in 'AI', 'Mechanics', 'Physics') {
                if ($function -eq 'Use') {
                    if ($name -eq 'Physics') { $function = 'Attach' } else { $function = 'Enter' }
                } elseif ($function -eq 'Release') {
                    if ($name -eq 'Physics') { $function = 'Detach' } else { $function = 'Exit' }
                }
            }
            $expected.Add($function)
            $called.Add($function)
        }
    }
    $source = Get-Content "$root/Resources/Modules/Appeal_Module_$name/Test/src/main.cpp" -Raw
    foreach ($function in [regex]::Matches($source, 'extern "C" \S+ (\w+)\(')) {
        if (-not $called.Contains($function.Groups[1].Value)) {
            throw "Missing smoke call for $name $($function.Groups[1].Value)"
        }
    }
}
$start = New-Object System.Diagnostics.ProcessStartInfo
$start.FileName = Join-Path $root 'Appeal.exe'
$start.Arguments = 'Test/New_Modules_Install_Smoke'
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
        throw 'New module smoke tests timed out.'
    }
    $output = $stdout.GetAwaiter().GetResult()
    $errors = $stderr.GetAwaiter().GetResult()
    $actual = @($output -split '\r?\n' | Where-Object { $_ -ne '' })
    if ($process.ExitCode -ne 0 -or $errors -ne '' -or
        ($actual -join "`n") -cne ($expected -join "`n")) {
        throw "Unexpected module output:`n$output`n$errors"
    }
    Write-Output "AI, Storage, Package, World, Files, Mechanics, and Physics smoke tests passed ($($expected.Count) calls including lifecycle hooks)."
} finally {
    $process.Dispose()
}
