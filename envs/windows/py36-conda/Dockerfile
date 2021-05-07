# escape=`

FROM mcr.microsoft.com/windows/servercore:1809
SHELL ["cmd", "/S", "/C"]

ADD https://go.microsoft.com/fwlink/?linkid=2088631 C:\temp\ndp48-x86-x64-allos-enu.exe
RUN C:\temp\ndp48-x86-x64-allos-enu.exe /q /norestart /install

ADD https://aka.ms/vs/16/release/vs_buildtools.exe C:\temp\vs_buildtools.exe
RUN C:\temp\vs_buildtools.exe --quiet --wait --norestart --nocache `
--installPath C:\BuildTools `
--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended `
--add Microsoft.VisualStudio.Component.VC.14.23.x86.x64 `
|| IF "%ERRORLEVEL%"=="3010" EXIT 0

ADD https://github.com/git-for-windows/git/releases/download/v2.31.1.windows.1/Git-2.31.1-64-bit.exe C:\temp\git-installer.exe
RUN C:\temp\git-installer.exe /VERYSILENT /NORESTART /NOCANCEL

ADD https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe C:\temp\miniconda.exe
RUN C:\temp\miniconda.exe /S /AddToPath=1 /InstallationType=JustMe /RegisterPython=0 /D=C:\temp\conda

WORKDIR C:\temp\pyprt

COPY envs/windows/py36-conda/environment.yml ./environment.yml
RUN conda env create -n pyprt --file environment.yml

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.23", "&&", "call", "conda.bat", "activate", "pyprt", "&&" ]
