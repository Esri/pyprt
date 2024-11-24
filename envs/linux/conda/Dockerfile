ARG REGISTRY
ARG BASE_TAG=linux
FROM ${REGISTRY}pyprt-base:${BASE_TAG}

# see https://github.com/conda/conda-docker/blob/master/miniconda3/centos7/Dockerfile
ADD https://repo.anaconda.com/miniconda/Miniconda3-py312_24.7.1-0-Linux-x86_64.sh /tmp/miniconda.sh
RUN bash /tmp/miniconda.sh -bfp /opt/conda && rm -rf /tmp/miniconda.sh

ARG PY_VER
COPY envs/linux/conda/environment-py${PY_VER}.yml /tmp/pyprt-conda-environment.yml
RUN /opt/conda/bin/conda update --quiet conda \
    && /opt/conda/bin/conda clean --all --yes \
    && /opt/conda/bin/conda config --system --set remote_backoff_factor 2 --set remote_connect_timeout_secs 60 --set remote_max_retries 8 --set remote_read_timeout_secs 240 \
    && /opt/conda/bin/conda config --system --add pkgs_dirs /tmp/conda_pkgs \
    && /opt/conda/bin/conda env create --quiet --prefix /tmp/pyprt-conda-env --file /tmp/pyprt-conda-environment.yml \
    && /opt/conda/bin/conda install --quiet --prefix /tmp/pyprt-conda-env -c esri arcgis \
    && /opt/conda/bin/conda update --quiet --prefix /tmp/pyprt-conda-env --all \
    && rm /tmp/pyprt-conda-environment.yml

# also see https://github.com/sclorg/devtoolset-container
RUN echo "unset BASH_ENV PROMPT_COMMAND ENV && source scl_source enable gcc-toolset-11 && source /opt/conda/bin/activate /tmp/pyprt-conda-env" >> /tmp/env_enable
ENV BASH_ENV=/tmp/env_enable ENV=/tmp/env_enable PROMPT_COMMAND="source /tmp/env_enable"
