# escape=`

ARG REGISTRY
ARG BASE_TAG=windows
FROM ${REGISTRY}pyprt-base:${BASE_TAG}
SHELL ["cmd", "/S", "/C"]

ADD https://repo.anaconda.com/miniconda/Miniconda3-py313_25.9.1-3-Windows-x86_64.exe C:/temp/miniconda.exe
RUN start /wait C:\temp\miniconda.exe /S /AddToPath=1 /InstallationType=JustMe /RegisterPython=0 /D=C:\temp\conda

WORKDIR C:/temp/pyprt

ARG PY_VER
COPY envs/windows/conda/environment-py${PY_VER}.yml ./environment.yml

# do not run conda update for now until this is fixed: https://github.com/conda/conda/issues/14322
RUN conda config --system --set remote_backoff_factor 2 --set remote_connect_timeout_secs 20 --set remote_max_retries 8 --set remote_read_timeout_secs 120 `
  && conda config --system --set conda_build.pkg_format tar.bz2 `
  && conda config --set always_yes true `
  && conda config --set show_channel_urls true `
  && conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main `
  && conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r `
  && conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/msys2 `
  && conda env create --quiet -n pyprt --file environment.yml `
  && conda install --quiet -n pyprt -c esri arcgis `
  && conda update --quiet -n pyprt --all `
  && conda clean --all --yes

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.38", "&&", "call", "conda.bat", "activate", "pyprt", "&&" ]
