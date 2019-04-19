include(FetchContent)


### look for the PRT libraries

# prt_DIR must point to the cmake subfolder of the desired SDK installation
if(NOT prt_DIR)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(PRT_CLS "rhel7-gcc63-x86_64-rel-dbg")
	else()
		set(PRT_CLS "rhel7-gcc63-x86_64-rel-opt")
	endif()

	FetchContent_Declare(prt
		URL "http://zrh-nexus.esri.com:8081/nexus/service/local/artifact/maven/content?r=CityEngine&g=prt.ce20190&v=LATEST&a=ce_sdk&c=${PRT_CLS}&e=zip"
	)

	FetchContent_GetProperties(prt)
	if(NOT prt_POPULATED)
	  FetchContent_Populate(prt)
	endif()

	set(prt_DIR "${prt_SOURCE_DIR}/cmake")
endif()

find_package(prt CONFIG REQUIRED)
message(STATUS "Using PRT ${PRT_VERSION_MAJOR}.${PRT_VERSION_MINOR}.${PRT_VERSION_MICRO} at ${prt_DIR}")


### look for PyBind11

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
