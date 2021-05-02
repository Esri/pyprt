# escape=`

# https://docs.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2019
# https://docs.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=vs-2019

ARG PY_VER=3.6

FROM python:${PY_VER}-windowsservercore-1809
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

WORKDIR C:\temp\pyprt
COPY ./envs/windows/requirements-py${PY_VER}.txt ./requirements.txt

RUN pip install --no-cache-dir --upgrade pip `
	&& pip install --no-cache-dir --upgrade wheel `
	&& pip install --no-cache-dir -r requirements.txt

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.23", "&&" ]
