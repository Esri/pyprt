###
### common helpers for both codec and client projects
###

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
message("Using PRT from: ${prt_DIR}")

find_package(prt CONFIG REQUIRED)
set(CESDK_VERSION "cesdk_${PRT_VERSION_MAJOR}_${PRT_VERSION_MINOR}_${PRT_VERSION_MICRO}")
message("Found PRT: ${CESDK_VERSION}")


### plugin installation location

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../install" CACHE PATH "default install prefix" FORCE)
endif()
message(STATUS "Installing into ${CMAKE_INSTALL_PREFIX}")
