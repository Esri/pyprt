FROM centos:7

RUN yum install -y centos-release-scl epel-release
RUN yum install -y git cmake3 ninja-build devtoolset-9-toolchain

RUN alternatives --install /usr/local/bin/cmake cmake /usr/bin/cmake3 20 \
    --slave /usr/local/bin/ctest ctest /usr/bin/ctest3 \
    --slave /usr/local/bin/cpack cpack /usr/bin/cpack3 \
    --slave /usr/local/bin/ccmake ccmake /usr/bin/ccmake3 \
    --family cmake

# no RPM repos yet, we manually build Python 3.9 with default CentOS 7 toolchain
WORKDIR /tmp
ADD https://www.python.org/ftp/python/3.9.6/Python-3.9.6.tar.xz python_src.tar.xz
RUN yum install -y gcc make openssl-devel libffi-devel bzip2-devel liblzma-dev
RUN tar xf python_src.tar.xz && \
    pushd Python-3.9.6 && \
    ./configure --enable-optimizations && \
    CPU_COUNT=$(grep -c ^processor /proc/cpuinfo) && \
    make -j$CPU_COUNT altinstall && \
    popd && \
    python3.9 -V

## this container works with mounted host directories
## we create a user with matching uid/gid to avoid permissions issues
## inspired by https://vsupalov.com/docker-shared-permissions/
ARG USER_ID
ARG GROUP_ID
RUN groupadd --gid $GROUP_ID user
RUN useradd --uid $USER_ID --gid $GROUP_ID user
USER user

## also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable devtoolset-9" >> /tmp/scl_enable
ENV BASH_ENV=/tmp/scl_enable ENV=/tmp/scl_enable PROMPT_COMMAND=". /tmp/scl_enable"

WORKDIR /tmp/pyprt/docker
COPY envs/centos7/py39/requirements.txt ./requirements.txt

ENV VIRTUAL_ENV=/tmp/pyprt-venv
RUN python3.9 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

RUN python -V && \
    python -m pip install --no-cache-dir --upgrade pip && python -m pip install --no-cache-dir --upgrade wheel && \
    python -m pip install --no-cache-dir -r requirements.txt
