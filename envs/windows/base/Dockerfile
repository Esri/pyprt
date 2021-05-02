# escape=`

FROM mcr.microsoft.com/windows/servercore:1809
SHELL ["cmd", "/S", "/C"]

ADD https://zrh-code.esri.com/nexus/content/repositories/thirdparty/com/microsoft/ndp/48/ndp-48-x86-x64-allos-enu.exe C:\temp\ndp48-x86-x64-allos-enu.exe
RUN C:\temp\ndp48-x86-x64-allos-enu.exe /q /norestart /install

ADD https://zrh-code.esri.com/nexus/content/repositories/thirdparty/com/microsoft/vs_buildtools/16/vs_buildtools-16.exe C:\temp\vs_buildtools.exe
RUN C:\temp\vs_buildtools.exe --quiet --wait --norestart --nocache `
    --installPath C:\BuildTools `
    --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended `
    --add Microsoft.VisualStudio.Component.VC.14.27.x86.x64 `
    || IF "%ERRORLEVEL%"=="3010" EXIT 0

ADD https://zrh-code.esri.com/nexus/content/repositories/thirdparty/com/github/git-for-windows/2.31.1/git-for-windows-2.31.1-64-bit.exe C:\temp\git-installer.exe
RUN C:\temp\git-installer.exe /VERYSILENT /NORESTART /NOCANCEL
