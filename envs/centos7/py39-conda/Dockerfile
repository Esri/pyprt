FROM centos:7

RUN yum install -y centos-release-scl-rh epel-release \
	&& yum install -y git cmake3 ninja-build devtoolset-9-toolchain \
	&& yum install -y curl bzip2

# make cmake3 the default cmake
RUN alternatives --install /usr/local/bin/cmake cmake /usr/bin/cmake3 20 \
	--slave /usr/local/bin/ctest ctest /usr/bin/ctest3 \
	--slave /usr/local/bin/cpack cpack /usr/bin/cpack3 \
	--slave /usr/local/bin/ccmake ccmake /usr/bin/ccmake3 \
	--family cmake

# this container works with mounted host directories
# we create a user with matching uid/gid to avoid permissions issues
# inspired by https://vsupalov.com/docker-shared-permissions/
ARG USER_ID
ARG GROUP_ID
RUN groupadd --gid $GROUP_ID user
RUN useradd --uid $USER_ID --gid $GROUP_ID user
USER user

RUN mkdir /tmp/pyprt
WORKDIR /tmp/pyprt

# see https://github.com/conda/conda-docker/blob/master/miniconda3/centos7/Dockerfile
RUN curl -sSL https://repo.anaconda.com/miniconda/Miniconda3-py39_4.10.3-Linux-x86_64.sh -o /tmp/pyprt/miniconda.sh \
    && bash ./miniconda.sh -bfp ./conda \
    && rm -rf ./miniconda.sh \
    && ./conda/bin/conda install -y python=3.9 \
    && ./conda/bin/conda update conda \
    && ./conda/bin/conda clean --all --yes

COPY envs/centos7/py39-conda/environment.yml ./environment.yml
RUN ./conda/bin/conda env create --prefix ./pyprt-conda-env --file ./environment.yml

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable devtoolset-9 && source /tmp/pyprt/conda/bin/activate /tmp/pyprt/pyprt-conda-env" >> /tmp/env_enable
ENV BASH_ENV=/tmp/env_enable ENV=/tmp/env_enable PROMPT_COMMAND="source /tmp/env_enable"
