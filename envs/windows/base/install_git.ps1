Write-Host '[1/3] Downloading Git...'
Invoke-WebRequest -Uri 'https://github.com/git-for-windows/git/releases/download/v2.52.0.windows.1/Git-2.52.0-64-bit.exe' -OutFile 'git_installer.exe'

Write-Host '[2/3] Installing Git...'
$gitId = Start-Process 'git_installer.exe' -ArgumentList '/VERYSILENT /NORESTART /NOCANCEL' -NoNewWindow -PassThru
$gitId.WaitForExit()
$children = Get-CimInstance Win32_Process | Where-Object { $_.ParentProcessId -eq $gitId.Id }
foreach ($child in $children) {
    Write-Host "Waiting for child process ID $($child.ProcessId)..."
    try { Wait-Process -Id $child.ProcessId -ErrorAction SilentlyContinue } catch {}
}

Write-Host '[3/3] Cleaning up...'
Remove-Item "git_installer.exe"
