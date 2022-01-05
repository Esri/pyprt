# escape=`

ARG REGISTRY=''
ARG PY_VER
FROM ${REGISTRY}pyprt-base:jnk-v5-windows-py${PY_VER}

WORKDIR C:\temp\pyprt
ARG PY_VER
COPY ./envs/windows/wheel/requirements-py${PY_VER}.txt ./requirements.txt

RUN pip install --no-cache-dir --upgrade pip `
	&& pip install --no-cache-dir --upgrade wheel `
	&& pip install --no-cache-dir -r requirements.txt

# https://devblogs.microsoft.com/cppblog/using-msvc-in-a-docker-container-for-your-c-projects/
ENTRYPOINT [ "C:\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat", "x64", "-vcvars_ver=14.27", "&&" ]
