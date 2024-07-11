ARG REGISTRY
ARG BASE_TAG=linux
FROM ${REGISTRY}pyprt-base:${BASE_TAG}

ARG PY_VER

# install python and any devel packages needed to install the required python modules
RUN set -e \
    && dnf -y update \
    && INSTALL_PKGS="libffi-devel bzip2-devel xz-devel krb5-devel" \
    && dnf install -y --setopt=tsflags=nodocs $INSTALL_PKGS \
    && rpm -V $INSTALL_PKGS \
    && dnf install -y python${PY_VER//./} python${PY_VER//./}-devel python${PY_VER//./}-pip \
    && dnf clean all && rm -rf /var/cache/dnf

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable gcc-toolset-11" >> /tmp/scl_enable
ENV BASH_ENV=/tmp/scl_enable ENV=/tmp/scl_enable PROMPT_COMMAND=". /tmp/scl_enable"

COPY envs/linux/wheel/requirements-py${PY_VER}.txt /tmp/pyprt/requirements.txt

ENV VIRTUAL_ENV=/tmp/pyprt/venv
RUN python${PY_VER} -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

RUN python -V \
    && python -m pip install --no-cache-dir --upgrade pip \
	&& python -m pip install --no-cache-dir --upgrade wheel \
	&& python -m pip install --no-cache-dir -r /tmp/pyprt/requirements.txt \
    && rm /tmp/pyprt/requirements.txt
