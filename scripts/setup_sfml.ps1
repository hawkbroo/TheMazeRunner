# SFML setup for The Maze Runner (PowerShell)
# Searches for *SFML*.zip in Downloads/Desktop, extracts to third_party\SFML,
# generates sfml_config.props for Visual Studio.

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$SfmlDest    = Join-Path $ProjectRoot "third_party\SFML"
$TempDir     = Join-Path $ProjectRoot "third_party\_extract"
$Report      = Join-Path $ProjectRoot "sfml_setup_report.txt"

function Write-Report([string[]]$lines) {
    $lines | Set-Content -Path $Report -Encoding UTF8
}

try {
    New-Item -ItemType Directory -Force -Path (Split-Path $SfmlDest) | Out-Null

    $searchPaths = @(
        (Join-Path $env:USERPROFILE "Downloads"),
        (Join-Path $env:USERPROFILE "Desktop"),
        $ProjectRoot
    )

    $zip = $null
    foreach ($dir in $searchPaths) {
        if (-not (Test-Path $dir)) { continue }
        $found = Get-ChildItem -Path $dir -Filter "*SFML*.zip" -File -ErrorAction SilentlyContinue |
            Sort-Object LastWriteTime -Descending |
            Select-Object -First 1
        if ($found) { $zip = $found; break }
    }

    $downloadUrl = $null
    if (-not $zip) {
        $downloadUrl = "https://www.sfml-dev.org/files/SFML-2.6.1-windows-vc17-64-bit.zip"
        $zipPath = Join-Path $ProjectRoot "third_party\sfml.zip"
        Write-Host "SFML zip not found. Downloading SFML 2.6.1..."
        Invoke-WebRequest -Uri $downloadUrl -OutFile $zipPath -UseBasicParsing
        $zip = Get-Item $zipPath
    }

    if (Test-Path $TempDir)  { Remove-Item $TempDir  -Recurse -Force }
    if (Test-Path $SfmlDest) { Remove-Item $SfmlDest -Recurse -Force }
    New-Item -ItemType Directory -Force -Path $TempDir | Out-Null

    Write-Host "Extracting: $($zip.FullName)"
    Expand-Archive -Path $zip.FullName -DestinationPath $TempDir -Force

    # Most zips contain a single top-level folder like SFML-2.6.1/
    $inner = Get-ChildItem $TempDir -Directory | Select-Object -First 1
    if ($inner -and (Test-Path (Join-Path $inner.FullName "include\SFML\Graphics.hpp"))) {
        Move-Item $inner.FullName $SfmlDest
        Remove-Item $TempDir -Recurse -Force -ErrorAction SilentlyContinue
    } else {
        Move-Item $TempDir $SfmlDest
    }

    if (-not (Test-Path (Join-Path $SfmlDest "include\SFML\Graphics.hpp"))) {
        throw "SFML include not found after extraction: $SfmlDest\include\SFML\Graphics.hpp"
    }

    $binDir = Join-Path $SfmlDest "bin"
    $dlls = @()
    if (Test-Path $binDir) {
        $dlls = Get-ChildItem $binDir -Filter "sfml-*.dll" -File | Select-Object -ExpandProperty Name
    }

    $major = 2
    if ($dlls -match "sfml-graphics-3\.dll|sfml-graphics-d-3\.dll") { $major = 3 }
    elseif ($dlls -match "sfml-graphics-2\.dll|sfml-graphics-d-2\.dll") { $major = 2 }
    else { throw "Could not detect SFML major version from DLLs in: $binDir" }

    if ($major -eq 3) {
        throw "Detected SFML 3.x, but this project currently targets SFML 2.x. Download SFML 2.6.1 (VC++ 17, 64-bit) and rerun SETUP.bat."
    }

    $propsPath = Join-Path $ProjectRoot "sfml_config.props"
    $propsLines = @(
        "<?xml version=""1.0"" encoding=""utf-8""?>",
        "<Project>",
        "  <PropertyGroup>",
        "    <SFML_DIR>$SfmlDest</SFML_DIR>",
        "    <SFML_MAJOR>$major</SFML_MAJOR>",
        "  </PropertyGroup>",
        "</Project>"
    )
    $propsLines | Set-Content -Path $propsPath -Encoding UTF8

    Write-Report @(
        "STATUS=OK",
        "ZIP=$($zip.FullName)",
        "DOWNLOAD_URL=$downloadUrl",
        "SFML_DIR=$SfmlDest",
        "SFML_MAJOR=$major",
        "DLLS=$($dlls -join '; ')",
        "HAS_GRAPHICS_DEBUG=$([bool](Test-Path (Join-Path $binDir 'sfml-graphics-d-2.dll')))"
    )

    Write-Host ""
    Write-Host "OK. SFML installed at: $SfmlDest" -ForegroundColor Green
    Write-Host "Open TheMazeRunner.sln and press Ctrl+F5." -ForegroundColor Green
}
catch {
    Write-Report @("STATUS=ERROR", "MESSAGE=$($_.Exception.Message)")
    Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
