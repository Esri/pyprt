# escape=`

ARG REGISTRY=''
ARG BASE_TAG=''
ARG PY_VER
FROM ${REGISTRY}pyprt-base:${BASE_TAG}-py${PY_VER}

WORKDIR C:\temp\pyprt
ARG PY_VER
COPY ./envs/windows/wheel/requirements-py${PY_VER::-2}.txt ./requirements.txt

RUN python -m pip install --no-cache-dir --upgrade pip `
	&& python -m pip install --no-cache-dir --upgrade wheel `
	&& python -m pip install --no-cache-dir -r requirements.txt

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.27", "&&" ]
