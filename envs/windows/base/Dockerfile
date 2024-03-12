# escape=`

ARG BASE_IMAGE="mcr.microsoft.com/windows/servercore:1809"
# or with python base image: python:3.11-windowsservercore-1809

FROM ${BASE_IMAGE}
SHELL ["cmd", "/S", "/C"]

# see https://github.com/microsoft/dotnet-framework-docker/blob/main/src/runtime/4.8/windowsservercore-ltsc2019/Dockerfile
ADD https://download.visualstudio.microsoft.com/download/pr/2d6bb6b2-226a-4baa-bdec-798822606ff1/8494001c276a4b96804cde7829c04d7f/ndp48-x86-x64-allos-enu.exe C:\temp\ndp48-x86-x64-allos-enu.exe
RUN C:\temp\ndp48-x86-x64-allos-enu.exe /q /norestart /install

# see https://learn.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2019
ADD https://aka.ms/vs/16/release/vs_buildtools.exe C:\temp\vs_buildtools.exe
RUN C:\temp\vs_buildtools.exe --quiet --wait --norestart --nocache `
    --installPath C:\BuildTools `
    --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended `
    --add Microsoft.VisualStudio.Component.VC.14.27.x86.x64 `
    || IF "%ERRORLEVEL%"=="3010" EXIT 0

ADD https://github.com/git-for-windows/git/releases/download/v2.31.1.windows.1/Git-2.31.1-64-bit.exe C:\temp\git-installer.exe
RUN C:\temp\git-installer.exe /VERYSILENT /NORESTART /NOCANCEL
