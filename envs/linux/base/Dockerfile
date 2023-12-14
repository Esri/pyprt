FROM centos:7

RUN yum install -y centos-release-scl-rh epel-release \
	&& yum install -y git ninja-build devtoolset-9-toolchain \
	&& yum install -y curl bzip2

# we manually install the tools below because we require newer versions than what the distro can provide

ADD https://zrh-code.esri.com/nexus/service/local/repositories/tools/content/com/esri/cityengine/toolchain/cmake/3.27.7/cmake-3.27.7-linux-x86_64.tar.gz /tmp/cmake.tar.gz
RUN tar -xzf /tmp/cmake.tar.gz -C /opt && rm /tmp/cmake.tar.gz
ENV PATH="/opt/cmake-3.27.7-linux-x86_64/bin:$PATH"

ADD https://github.com/NixOS/patchelf/releases/download/0.18.0/patchelf-0.18.0-x86_64.tar.gz /tmp/patchelf.tar.gz
RUN mkdir -p /opt/patchelf-0.18.0-x86_64 && tar -xzf /tmp/patchelf.tar.gz -C /opt/patchelf-0.18.0-x86_64 && rm /tmp/patchelf.tar.gz
ENV PATH="/opt/patchelf-0.18.0-x86_64/bin:$PATH"