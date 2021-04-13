FROM centos:7

RUN yum install -y centos-release-scl-rh epel-release

RUN yum install -y git cmake3 ninja-build devtoolset-8-toolchain \
rh-python38-python rh-python38-python-devel rh-python38-python-pip

RUN alternatives --install /usr/local/bin/cmake cmake /usr/bin/cmake3 20 \
--slave /usr/local/bin/ctest ctest /usr/bin/ctest3 \
--slave /usr/local/bin/cpack cpack /usr/bin/cpack3 \
--slave /usr/local/bin/ccmake ccmake /usr/bin/ccmake3 \
--family cmake

USER 997:995

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable devtoolset-8" >> /tmp/scl_enable
ENV BASH_ENV=/tmp/scl_enable ENV=/tmp/scl_enable PROMPT_COMMAND=". /tmp/scl_enable"

WORKDIR /tmp/pyprt/docker
COPY envs/centos7/py38/requirements.txt ./requirements.txt

ENV VIRTUAL_ENV=/tmp/pyprt-venv
RUN source scl_source enable rh-python38 && python -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

RUN python -m pip install --no-cache-dir --upgrade pip && python -m pip install --no-cache-dir --upgrade wheel
RUN python -m pip install --no-cache-dir -r requirements.txt
