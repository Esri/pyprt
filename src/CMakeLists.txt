cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)
include(FetchContent)

### project definition

project(pyprt)
include(dependencies.cmake)


set(CLIENT_TARGET pyprt)
set(CODEC_TARGET pyprt_codec)


FetchContent_Declare(
 	pybind11
	GIT_REPOSITORY https://github.com/pybind/pybind11.git
	GIT_TAG v2.2.4
)

FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
	FetchContent_Populate(pybind11)
	add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()

add_subdirectory(codec)
add_subdirectory(client)

add_dependencies(${CLIENT_TARGET} ${CODEC_TARGET})


### setup installation target

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../install" CACHE PATH "default install prefix" FORCE)
	message(STATUS "Set default install prefix: ${CMAKE_INSTALL_PREFIX}")
endif()
