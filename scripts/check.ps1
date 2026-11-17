[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release', 'Both')]
    [string]$Configuration = 'Both',
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[A-Za-z0-9][A-Za-z0-9_.-]*$')]
    [string]$Preset = $(if ([string]::IsNullOrWhiteSpace($env:LINSCALER_PRESET)) {
        'msvc'
    } else {
        $env:LINSCALER_PRESET
    })
)
$ErrorActionPreference = 'Stop'
if ($env:OS -ne 'Windows_NT') {
    throw 'This script requires Windows. On other platforms use the CMake presets.'
}
$projectRoot = Split-Path -Parent $PSScriptRoot

function Find-SharedCMake {
    if (-not [string]::IsNullOrWhiteSpace($env:LINSCALER_CMAKE)) {
        if (-not (Test-Path -LiteralPath $env:LINSCALER_CMAKE -PathType Leaf)) {
            throw 'LINSCALER_CMAKE must point to an installed cmake.exe outside this checkout.'
        }
        return (Resolve-Path -LiteralPath $env:LINSCALER_CMAKE).ProviderPath
    }
    $cmakeCommand = Get-Command cmake.exe -CommandType Application -ErrorAction SilentlyContinue |
        Select-Object -First 1
    if ($cmakeCommand) { return $cmakeCommand.Source }

    $vswhereCommand = Get-Command vswhere.exe -CommandType Application -ErrorAction SilentlyContinue |
        Select-Object -First 1
    $vswhere = if ($vswhereCommand) {
        $vswhereCommand.Source
    } elseif (${env:ProgramFiles(x86)}) {
        Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    }
    if ($vswhere -and (Test-Path -LiteralPath $vswhere -PathType Leaf)) {
        $candidates = @(& $vswhere -latest -products '*' -version '[17.0,18.0)' `
            -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
                Microsoft.VisualStudio.Component.VC.CMake.Project `
            -find 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe')
        if ($LASTEXITCODE -eq 0) {
            foreach ($candidate in $candidates) {
                if (Test-Path -LiteralPath $candidate -PathType Leaf) { return $candidate }
            }
        }
    }
    throw 'Shared CMake not found. Install CMake or the Visual Studio 2022 C++ CMake tools outside this checkout, then set LINSCALER_CMAKE or PATH.'
}

$cmake = Find-SharedCMake
$projectPrefix = [IO.Path]::GetFullPath($projectRoot).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
if ([IO.Path]::GetFullPath($cmake).StartsWith($projectPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Use a machine-installed CMake outside this checkout; project-local tools are not supported.'
}
$ctest = Join-Path (Split-Path -Parent $cmake) 'ctest.exe'
if (-not (Test-Path -LiteralPath $ctest -PathType Leaf)) {
    throw 'CTest was not found beside the selected CMake. Select a complete shared CMake installation.'
}

Push-Location $projectRoot
try {
    Write-Host "CMake: $cmake"
    Write-Host "Configure preset: $Preset"
    $configurations = if ($Configuration -eq 'Both') { @('Debug', 'Release') } else { @($Configuration) }
    & $cmake --preset $Preset
    $commandExitCode = $LASTEXITCODE
    if ($commandExitCode -ne 0) {
        Write-Error "Configure failed: $Preset" -ErrorAction Continue
        exit $commandExitCode
    }
    foreach ($item in $configurations) {
        $buildPreset = $Preset + '-' + $item.ToLowerInvariant()
        & $cmake --build --preset $buildPreset
        $commandExitCode = $LASTEXITCODE
        if ($commandExitCode -ne 0) {
            Write-Error "Build failed: $buildPreset" -ErrorAction Continue
            exit $commandExitCode
        }
        & $ctest --preset $buildPreset --output-on-failure
        $commandExitCode = $LASTEXITCODE
        if ($commandExitCode -ne 0) {
            Write-Error "Tests failed: $buildPreset" -ErrorAction Continue
            exit $commandExitCode
        }
    }
} finally {
    Pop-Location
}

