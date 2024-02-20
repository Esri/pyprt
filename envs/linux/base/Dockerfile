FROM centos:7

RUN yum -y update \
    && yum install -y centos-release-scl-rh epel-release \
	&& yum install -y git ninja-build devtoolset-9-toolchain \
	&& yum install -y curl bzip2 \
    && yum clean all \
    && rm -rf /var/cache/yum

# we manually install the tools below because we require newer versions than what the distro can provide

ADD https://zrh-code.esri.com/nexus/service/local/repositories/tools/content/com/esri/cityengine/toolchain/cmake/3.27.7/cmake-3.27.7-linux-x86_64.tar.gz /tmp/cmake.tar.gz
RUN tar -xzf /tmp/cmake.tar.gz -C /opt && rm /tmp/cmake.tar.gz
ENV PATH="/opt/cmake-3.27.7-linux-x86_64/bin:$PATH"

ADD https://github.com/NixOS/patchelf/releases/download/0.18.0/patchelf-0.18.0-x86_64.tar.gz /tmp/patchelf.tar.gz
RUN mkdir -p /opt/patchelf-0.18.0-x86_64 && tar -xzf /tmp/patchelf.tar.gz -C /opt/patchelf-0.18.0-x86_64 && rm /tmp/patchelf.tar.gz
ENV PATH="/opt/patchelf-0.18.0-x86_64/bin:$PATH"

# setup container initialization and user

ENV HOME=/tmp/pyprt
COPY envs/linux/base/docker-entrypoint.sh /usr/bin/container-entrypoint.sh
RUN set -e \
    && mkdir -p ${HOME} \
    && groupadd -r default -f -g 1001 \
    && useradd -r default -g default -d ${HOME} -s /sbin/nologin -c "Default Application User" \
    && chown default:default /usr/bin/container-entrypoint.sh \
    && chmod 555 /usr/bin/container-entrypoint.sh
WORKDIR ${HOME}
ENTRYPOINT ["container-entrypoint.sh"]
