#!/usr/bin/env pwsh

# Usage:
# save on desktop
# Win + R
# powershell -ExecutionPolicy Bypass -File %UserProfile%\Desktop\pd-install.ps1

# Stop executing script on any error
$ErrorActionPreference = 'Stop'
# Do not show download progress
$ProgressPreference = 'SilentlyContinue'

# Taken from https://stackoverflow.com/a/34559554/6537420
function New-TemporaryDirectory {
	$parent = [System.IO.Path]::GetTempPath()
	[string] $name = [System.Guid]::NewGuid()
	New-Item -ItemType Directory -Path (Join-Path $parent $name)
}

function Format-Parameters {
	Param ([hashtable]$NamedParameters)
	return ($NamedParameters.GetEnumerator() | ForEach-Object { "-$($_.Key) `"$($_.Value)`"" }) -join " "
}

# Self-elevate the script if required
if (-Not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] 'Administrator')) {
 if ([int](Get-CimInstance -Class Win32_OperatingSystem | Select-Object -ExpandProperty BuildNumber) -ge 6000) {
  $CommandLine = "-ExecutionPolicy Bypass -File `"" + $MyInvocation.MyCommand.Path + "`" " + (Format-Parameters $MyInvocation.BoundParameters) + " " + $MyInvocation.UnboundArguments
  Start-Process -FilePath PowerShell.exe -Verb Runas -ArgumentList $CommandLine
  Exit
 }
}

# Remainder of script here

$tempFileFolder = New-TemporaryDirectory

Write-Output "Temporary dir: $tempFileFolder"

Write-Output "Creating necessary folders"
mkdir "$env:APPDATA\UnrealVRMod\PenDriverPro-Win64-Shipping" -Force
mkdir "$env:APPDATA\raicuparta\rai-pal\data\mod-loaders\runnable\mods\chihuahua" -Force

Write-Output "Adding Windows Defender exceptions"
Add-MpPreference -ExclusionPath "$env:APPDATA\UnrealVRMod" -Force
Add-MpPreference -ExclusionPath "$env:APPDATA\raicuparta\rai-pal\data\mod-loaders\runnable" -Force

Write-Output "Downloading Rai Pal"
Invoke-WebRequest https://github.com/Raicuparta/rai-pal/releases/latest/download/RaiPal.msi -OutFile "$tempFileFolder/RaiPal.msi"

Write-Output "Downloading Chihuahua"
Invoke-WebRequest https://github.com/keton/chihuahua/releases/latest/download/chihuahua.zip -OutFile "$tempFileFolder/chihuahua.zip"

Write-Output "Downloading Pacific Drive profile"
Invoke-WebRequest https://github.com/keton/pacific-drive-uevr/releases/latest/download/PenDriverPro-Win64-Shipping.zip -OutFile "$tempFileFolder/PenDriverPro-Win64-Shipping.zip"

Write-Output "Installing Rai Pal"
Start-Process -NoNewWindow -Wait msiexec -ArgumentList /i, "$tempFileFolder\RaiPal.msi", /qn

Write-Output "Installing chihuahua"
Expand-Archive -LiteralPath "$tempFileFolder/chihuahua.zip" -DestinationPath "$env:APPDATA\raicuparta\rai-pal\data\mod-loaders\runnable\mods\chihuahua" -Force

Write-Output "Setting chihuahua to pull UEVR nightlies"
(Get-Content "$env:APPDATA\raicuparta\rai-pal\data\mod-loaders\runnable\mods\chihuahua\rai-pal-manifest.json").Replace('"--delay"', '"--uevr-build","Nightly","--delay"') | Set-Content "$env:APPDATA\raicuparta\rai-pal\data\mod-loaders\runnable\mods\chihuahua\rai-pal-manifest.json"

Write-Output "Installing Pacific Drive profile"
Expand-Archive -LiteralPath "$tempFileFolder/PenDriverPro-Win64-Shipping.zip" -DestinationPath "$env:APPDATA\UnrealVRMod\PenDriverPro-Win64-Shipping" -Force

Write-Output "Cleaning up"
Remove-Item -Force -Recurse "$tempFileFolder"

Write-Host "Press any key to continue..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
