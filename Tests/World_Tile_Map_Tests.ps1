$ErrorActionPreference = 'Stop'
$Root = Split-Path $PSScriptRoot -Parent
$World = Join-Path $Root 'Resources/Modules/Appeal_Module_World'
& make -C (Join-Path $World 'Appeal_Standard_World') -j4 all test
if ($LASTEXITCODE -ne 0) { throw 'World native tests/build failed.' }
& make -C $World -j4
if ($LASTEXITCODE -ne 0) { throw 'World module build failed.' }
$Start = New-Object System.Diagnostics.ProcessStartInfo
$Start.FileName = Join-Path $Root 'Appeal.exe'
$Start.Arguments = 'Test/World_Tile_Map'
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
        throw 'World tile-map script timed out.'
    }
    $Output = $OutputTask.GetAwaiter().GetResult().Replace("`r", '').TrimEnd("`n")
    $Errors = $ErrorTask.GetAwaiter().GetResult()
    $Expected = @('Tree', '7', '42.000000', '74.000000', '2', 'Room', 'Room',
        'Tree', '7', 'FREE_ENTITY_PRESERVED', 'Shared_Tree', '', '', '',
        'WORLD_TILE_MAP_SCRIPT_PASSED') -join "`n"
    if ($Process.ExitCode -ne 0 -or $Errors -ne '' -or $Output -cne $Expected) {
        throw "World tile-map script failed:`n$Output`n$Errors"
    }
    Write-Output 'WORLD_TILE_MAP_SCRIPT_PASSED'
} finally { $Process.Dispose() }
