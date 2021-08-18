# escape=`

ARG REGISTRY=''
ARG PY_VER

FROM ${REGISTRY}pyprt-base:jnk-v4-windows

ADD https://zrh-code.esri.com/nexus/service/local/repositories/tools/content/io/conda/miniconda3/4.10.3/miniconda3-4.10.3-py39-windows-x86_64.exe C:\temp\miniconda.exe
RUN C:\temp\miniconda.exe /S /AddToPath=1 /InstallationType=JustMe /RegisterPython=0 /D=C:\temp\conda

WORKDIR C:\temp\pyprt

ARG PY_VER
COPY envs/windows/conda/environment-py${PY_VER}.yml ./environment.yml
RUN conda env create -n pyprt --file environment.yml

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.27", "&&", "call", "conda.bat", "activate", "pyprt", "&&" ]
