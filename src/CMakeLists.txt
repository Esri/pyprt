#
# PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
#
# Copyright (c) 2012-2021 Esri R&D Center Zurich
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.

cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)


### project definition

project(pyprt CXX)

include(dependencies.cmake)

set(CLIENT_TARGET pyprt)
set(CODEC_TARGET pyprt_codec)

add_subdirectory(codec)
add_subdirectory(client)

add_dependencies(${CLIENT_TARGET} ${CODEC_TARGET})


### setup default install location

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../install/pyprt" CACHE PATH "default install prefix" FORCE)
endif()
message(STATUS "Installing into ${CMAKE_INSTALL_PREFIX}")
