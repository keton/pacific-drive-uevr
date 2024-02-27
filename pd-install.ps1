#!/usr/bin/env pwsh

# Usage:
# Press Win + R
# powershell -nop -c "iex(New-Object Net.WebClient).DownloadString('https://raw.githubusercontent.com/keton/pacific-drive-uevr/master/pd-install.ps1')"

$elevatedScript = {
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
}

# Self-elevate the script
Start-Process -Verb RunAs powershell.exe -ArgumentList (
	'-EncodedCommand', (
		[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($elevatedScript))
	),
	'-ExecutionPolicy Bypass'
)
Exit
