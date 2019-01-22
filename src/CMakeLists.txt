cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)


### project definition

project(pyprt)
include(dependencies.cmake)

set(CLIENT_TARGET pyprt)
set(CODEC_TARGET pyprt_codec)


add_subdirectory(codec)
add_subdirectory(pybind11)
add_subdirectory(client)

add_dependencies(${CLIENT_TARGET} ${CODEC_TARGET})


### setup installation target

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../install" CACHE PATH "default install prefix" FORCE)
	message(STATUS "Set default install prefix: ${CMAKE_INSTALL_PREFIX}")
endif()
