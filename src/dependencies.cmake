include(FetchContent)


### environment

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(PYPRT_WINDOWS 1)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(PYPRT_LINUX 1)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(PYPRT_MACOS 1)
endif()


### look for the PRT libraries

# prt_DIR must point to the cmake subfolder of the desired SDK installation
if(NOT prt_DIR)
	if(PYPRT_WINDOWS)
		set(PRT_OS "win10")
		set(PRT_TC "vc141")
	elseif(PYPRT_LINUX)
		set(PRT_OS "rhel7")
		set(PRT_TC "gcc63")
	elseif(PYPRT_MACOS)
		set(PRT_OS "osx12")
		set(PRT_TC "ac81")
	endif()

	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(PRT_CC "dbg")
	else()
		set(PRT_CC "opt")
	endif()

	set(PRT_BRANCH  "ce20190")
	set(PRT_VERSION "LATEST") # latest version within above branch
	set(PRT_CLS    "${PRT_OS}-${PRT_TC}-x86_64-rel-${PRT_CC}")
	set(PRT_URL    "http://zrh-nexus.esri.com:8081/nexus/service/local/artifact/maven/content?r=CityEngine&g=prt.${PRT_BRANCH}&v=${PRT_VERSION}&a=ce_sdk&c=${PRT_CLS}&e=zip")

	FetchContent_Declare(prt URL ${PRT_URL})
	FetchContent_GetProperties(prt)
	if(NOT prt_POPULATED)
		message(STATUS "Fetching PRT from ${PRT_URL}...")
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
