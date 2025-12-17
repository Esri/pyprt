# escape=`

ARG REGISTRY
ARG BASE_TAG=windows
ARG PY_VER
FROM ${REGISTRY}pyprt-base:${BASE_TAG}-py${PY_VER}
SHELL ["cmd", "/S", "/C"]

WORKDIR C:/temp/work
ARG PY_VER
COPY ./envs/windows/wheel/requirements-py${PY_VER}.txt C:/temp/pyprt-wheel-requirements.txt

RUN python -m pip install --no-cache-dir --upgrade pip `
	&& python -m pip install --no-cache-dir --upgrade wheel `
	&& python -m pip install --no-cache-dir -r C:/temp/pyprt-wheel-requirements.txt

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.38", "&&" ]
