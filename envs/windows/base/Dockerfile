# escape=`

ARG BASE_IMAGE="mcr.microsoft.com/windows/servercore:1809"
# or with python base image: python:3.11-windowsservercore-1809

FROM ${BASE_IMAGE}
SHELL ["cmd", "/S", "/C"]

# see https://learn.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2022
ADD https://aka.ms/vs/17/release/vs_buildtools.exe C:\temp\vs_buildtools.exe
RUN C:\temp\vs_buildtools.exe --quiet --wait --norestart --nocache ^ `
    --installPath C:\BuildTools ^ `
    --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended ^ `
    --add Microsoft.VisualStudio.Component.VC.14.37.17.7.x86.x64 ^ `
    || IF "%ERRORLEVEL%"=="3010" EXIT 0

ADD https://github.com/git-for-windows/git/releases/download/v2.31.1.windows.1/Git-2.31.1-64-bit.exe C:\temp\git-installer.exe
RUN C:\temp\git-installer.exe /VERYSILENT /NORESTART /NOCANCEL
