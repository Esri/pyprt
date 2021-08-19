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
