ARG IMAGE_OS_BASE=almalinux
ARG IMAGE_OS_VERSION=8
FROM ${IMAGE_OS_BASE}:${IMAGE_OS_VERSION}

RUN set -e \
    && rpm --import https://repo.almalinux.org/almalinux/RPM-GPG-KEY-AlmaLinux \
    && dnf -y update \
    && INSTALL_PKGS="git unzip bzip2 gcc-toolset-11 gcc-toolset-11-gcc gcc-toolset-11-gcc-c++ gcc-toolset-11-gdb redhat-lsb make" \
    && dnf install -y --setopt=tsflags=nodocs $INSTALL_PKGS \
    && rpm -V $INSTALL_PKGS \
    && dnf clean all \
    && rm -rf /var/cache/dnf

# we manually install the tools below because we require newer versions than what the distro can provide

ADD https://cmake.org/files/v3.27/cmake-3.27.7-linux-x86_64.tar.gz /tmp/cmake.tar.gz
RUN tar -xzf /tmp/cmake.tar.gz -C /opt && rm /tmp/cmake.tar.gz
ENV PATH="/opt/cmake-3.27.7-linux-x86_64/bin:$PATH"

ADD https://github.com/NixOS/patchelf/releases/download/0.17.2/patchelf-0.17.2-x86_64.tar.gz /tmp/patchelf.tar.gz
RUN mkdir -p /opt/patchelf-0.17.2-x86_64 && tar -xzf /tmp/patchelf.tar.gz -C /opt/patchelf-0.17.2-x86_64 && rm /tmp/patchelf.tar.gz
ENV PATH="/opt/patchelf-0.17.2-x86_64/bin:$PATH"

# setup container initialization and user

ENV HOME=/tmp/work
COPY envs/linux/base/docker-entrypoint.sh /usr/bin/container-entrypoint.sh
RUN set -e \
    && mkdir -p ${HOME} \
    && groupadd -r default -f -g 1001 \
    && useradd -r default -g default -d ${HOME} -s /sbin/nologin -c "Default Application User" \
    && chown default:default /usr/bin/container-entrypoint.sh \
    && chmod 555 /usr/bin/container-entrypoint.sh
WORKDIR ${HOME}
ENTRYPOINT ["container-entrypoint.sh"]
