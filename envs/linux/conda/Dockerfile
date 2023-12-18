ARG REGISTRY=''
ARG BASE_TAG=''
FROM ${REGISTRY}pyprt-base:${BASE_TAG}

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
ADD --chown=${USER_ID}:${GROUP_ID} https://zrh-code.esri.com/nexus/service/local/repositories/tools/content/io/conda/miniconda3/22.11.1/miniconda3-22.11.1-py310-linux-x86_64.sh /tmp/pyprt/miniconda.sh
RUN bash miniconda.sh -bfp ./conda && \
    rm -rf ./miniconda.sh && \
    conda/bin/conda update conda && \
    conda/bin/conda clean --all --yes && \
    conda/bin/conda config -v --system --set remote_backoff_factor 2 --set remote_connect_timeout_secs 60 --set remote_max_retries 8 --set remote_read_timeout_secs 240

COPY envs/linux/conda/environment-py${PY_VER}.yml ./environment.yml
RUN ./conda/bin/conda env create -vv --prefix ./pyprt-conda-env --file ./environment.yml && \
    ./conda/bin/conda install --prefix ./pyprt-conda-env -c esri arcgis

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable devtoolset-9 && source /tmp/pyprt/conda/bin/activate /tmp/pyprt/pyprt-conda-env" >> /tmp/env_enable
ENV BASH_ENV=/tmp/env_enable ENV=/tmp/env_enable PROMPT_COMMAND="source /tmp/env_enable"
