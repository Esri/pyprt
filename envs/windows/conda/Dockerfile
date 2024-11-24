# escape=`

ARG REGISTRY
ARG BASE_TAG=windows
FROM ${REGISTRY}pyprt-base:${BASE_TAG}

ADD https://repo.anaconda.com/miniconda/Miniconda3-py312_24.7.1-0-Windows-x86_64.exe C:/temp/miniconda.exe
RUN C:\temp\miniconda.exe /S /AddToPath=1 /InstallationType=JustMe /RegisterPython=0 /D=C:\temp\conda

WORKDIR C:/temp/pyprt

ARG PY_VER
COPY envs/windows/conda/environment-py${PY_VER}.yml ./environment.yml
RUN conda update --quiet conda `
  && conda clean --all --yes `
  && conda config --system --set remote_backoff_factor 2 --set remote_connect_timeout_secs 20 --set remote_max_retries 8 --set remote_read_timeout_secs 120 `
  && conda config --system --set conda_build.pkg_format tar.bz2 `
  && conda config --system --add channels defaults `
  && conda env create --quiet -n pyprt --file environment.yml `
  && conda install --quiet -n pyprt -c esri arcgis `
  && conda update --quiet -n pyprt --all

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.37", "&&", "call", "conda.bat", "activate", "pyprt", "&&" ]
