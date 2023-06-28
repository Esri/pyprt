# escape=`

ARG REGISTRY=''
ARG PY_VER

ARG BASE_TAG=''
FROM ${REGISTRY}pyprt-base:${BASE_TAG} AS req_install

ADD https://zrh-code.esri.com/nexus/service/local/repositories/tools/content/io/conda/miniconda3/22.11.1/miniconda3-22.11.1-py310-windows-x86_64.exe C:\temp\miniconda.exe
RUN C:\temp\miniconda.exe /S /AddToPath=1 /InstallationType=JustMe /RegisterPython=0 /D=C:\temp\conda

WORKDIR C:\temp\pyprt

ARG PY_VER
COPY envs/windows/conda/environment-py${PY_VER}.yml ./environment.yml
RUN conda update conda
RUN conda clean --all --yes
RUN conda config -v --system --set remote_backoff_factor 2 --set remote_connect_timeout_secs 20 --set remote_max_retries 8 --set remote_read_timeout_secs 120
RUN conda env create -vv -n pyprt --file environment.yml

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.27", "&&", "call", "conda.bat", "activate", "pyprt", "&&" ]

FROM req_install AS arcgis_install
RUN conda install -c esri arcgis
