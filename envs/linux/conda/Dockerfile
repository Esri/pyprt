ARG REGISTRY
ARG BASE_TAG=linux
FROM ${REGISTRY}pyprt-base:${BASE_TAG}

# see https://github.com/conda/conda-docker/blob/master/miniconda3/centos7/Dockerfile
ADD https://repo.anaconda.com/miniconda/Miniconda3-py311_23.10.0-1-Linux-x86_64.sh /tmp/pyprt/miniconda.sh
RUN bash miniconda.sh -bfp ./conda && rm -rf ./miniconda.sh

ARG PY_VER
COPY envs/linux/conda/environment-py${PY_VER}.yml ./environment.yml
RUN conda/bin/conda update conda \
    && conda/bin/conda clean --all --yes \
    && conda/bin/conda config --system --set remote_backoff_factor 2 --set remote_connect_timeout_secs 60 --set remote_max_retries 8 --set remote_read_timeout_secs 240 \
    && conda/bin/conda env create --prefix ./pyprt-conda-env --file ./environment.yml \
    && conda/bin/conda install --prefix ./pyprt-conda-env -c esri arcgis \
    && conda/bin/conda update --prefix ./pyprt-conda-env --all

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable gcc-toolset-11 && source /tmp/pyprt/conda/bin/activate /tmp/pyprt/pyprt-conda-env" >> /tmp/env_enable
ENV BASH_ENV=/tmp/env_enable ENV=/tmp/env_enable PROMPT_COMMAND="source /tmp/env_enable"
