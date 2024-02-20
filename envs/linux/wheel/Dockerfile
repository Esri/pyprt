ARG REGISTRY=''
ARG BASE_TAG=''
FROM ${REGISTRY}pyprt-base:${BASE_TAG}

ARG PY_VER

# install python and any devel packages needed to install the required python modules
RUN yum -y update \
    && yum install -y gcc gcc-c++ make libffi-devel bzip2-devel liblzma-dev \
    && yum install -y rh-python${PY_VER//./}-python rh-python${PY_VER//./}-python-devel rh-python${PY_VER//./}-python-pip krb5-devel \
    && yum clean all \
    && rm -rf /var/cache/yum

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable devtoolset-9" >> /tmp/scl_enable
ENV BASH_ENV=/tmp/scl_enable ENV=/tmp/scl_enable PROMPT_COMMAND=". /tmp/scl_enable"

COPY envs/linux/wheel/requirements-py${PY_VER}.txt /tmp/pyprt/requirements.txt

ENV VIRTUAL_ENV=/tmp/pyprt/venv
RUN source scl_source enable rh-python${PY_VER//./} && python -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

RUN python -V \
    && python -m pip install --no-cache-dir --upgrade pip \
	&& python -m pip install --no-cache-dir --upgrade wheel \
	&& python -m pip install --no-cache-dir -r /tmp/pyprt/requirements.txt \
    && rm /tmp/pyprt/requirements.txt
