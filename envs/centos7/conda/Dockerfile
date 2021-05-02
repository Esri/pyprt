ARG REGISTRY=''
FROM ${REGISTRY}pyprt-base:jnk-v4-centos7

ARG PY_VER
ARG USER_ID
ARG GROUP_ID

# this container works with mounted host directories
# we create a user with matching uid/gid to avoid permissions issues
# inspired by https://vsupalov.com/docker-shared-permissions/
RUN groupadd --gid $GROUP_ID user
RUN useradd --uid $USER_ID --gid $GROUP_ID user
USER user

RUN mkdir /tmp/pyprt
WORKDIR /tmp/pyprt

# see https://github.com/conda/conda-docker/blob/master/miniconda3/centos7/Dockerfile
RUN curl -sSL https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -o /tmp/pyprt/miniconda.sh \
    && bash ./miniconda.sh -bfp ./conda \
    && rm -rf ./miniconda.sh \
    && ./conda/bin/conda install -y python=${PY_VER} \
    && ./conda/bin/conda update conda \
    && ./conda/bin/conda clean --all --yes

COPY envs/centos7/conda/environment-py${PY_VER}.yml ./environment.yml
RUN ./conda/bin/conda env create --prefix ./pyprt-conda-env --file ./environment.yml

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable devtoolset-9 && source /tmp/pyprt/conda/bin/activate /tmp/pyprt/pyprt-conda-env" >> /tmp/env_enable
ENV BASH_ENV=/tmp/env_enable ENV=/tmp/env_enable PROMPT_COMMAND="source /tmp/env_enable"
