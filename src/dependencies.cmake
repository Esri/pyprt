#
# PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
#
# Copyright (c) 2012-2022 Esri R&D Center Zurich
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

include(FetchContent)


### environment

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(PYPRT_WINDOWS 1)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(PYPRT_LINUX 1)
endif()


### read dependencies properties

function(get_dependencies_properties PROP_FILE)
	file(STRINGS "${PROP_FILE}" PROPS)
	foreach(P IN LISTS PROPS)
		string(FIND "${P}" "#" HASHPOS)
		if(NOT ${HASHPOS} EQUAL 0)
			string(REPLACE "=" ";" L "${P}")
			list(GET L 0 PN)
			list(GET L 1 PV)
			set(${PN} ${PV} PARENT_SCOPE)
		endif()
	endforeach()
endfunction()

# workaround: copy the file once to register it as a source dependency
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dependencies.properties dependencies.properties COPYONLY)
get_dependencies_properties(${CMAKE_CURRENT_BINARY_DIR}/dependencies.properties)


### look for the PRT libraries

# if prt_DIR is not provided, download PRT from its github home
if(NOT prt_DIR)
	if(PYPRT_WINDOWS)
		set(PRT_CLS "${PRT_CLS_WINDOWS}")
	elseif(PYPRT_LINUX)
		set(PRT_CLS "${PRT_CLS_LINUX}")
	endif()

	set(PRT_ARCHIVE "esri_ce_sdk-${PRT_VERSION}-${PRT_CLS}.zip")
	set(PRT_URL     "https://github.com/esri/cityengine-sdk/releases/download/${PRT_VERSION}/${PRT_ARCHIVE}")

	FetchContent_Declare(prt URL ${PRT_URL} DOWNLOAD_NO_EXTRACT $ENV{PRT_EXTRACTION_WORKAROUND})
	FetchContent_GetProperties(prt)
	if(NOT prt_POPULATED)
		message(STATUS "Fetching PRT from ${PRT_URL}...")
		FetchContent_Populate(prt)
	endif()
	if(prt_POPULATED)
		if($ENV{PRT_EXTRACTION_WORKAROUND})
			# on certain (linux only?) systems, "cmake -E tar" on PRT_ARCHIVE results in a corrupted PRT distribution
			# manually provide PRT_EXTRACTION_WORKAROUND=1 on the cmake cmd line to use this workaround
			# maybe related to https://bugzilla.redhat.com/show_bug.cgi?id=1526404
			message(STATUS "Active PRT extraction workaround: ${prt_SOURCE_DIR}/cmake")
			if (NOT EXISTS "${prt_SOURCE_DIR}/cmake")
				# yep, relying on cmake implementation details, better look away...
				set(PRT_LOCAL_ARCHIVE "${prt_SOURCE_DIR}/../prt-subbuild/prt-populate-prefix/src/${PRT_ARCHIVE}")
				execute_process(COMMAND unzip -q -d ${prt_SOURCE_DIR} ${PRT_LOCAL_ARCHIVE})
			endif()
		endif()
		set(prt_DIR "${prt_SOURCE_DIR}/cmake")
	endif()
endif()

find_package(prt CONFIG REQUIRED)
message(STATUS "Using PRT ${PRT_VERSION_MAJOR}.${PRT_VERSION_MINOR}.${PRT_VERSION_MICRO} at ${prt_DIR}")

# workaround omission in prtConfig.cmake: manually setting up a resources list which needs to be installed as well
list(APPEND PRT_EXT_RESOURCES ${PRT_EXTENSION_PATH}/usd)


### look for PyBind11

if(pybind11_DIR)
	add_subdirectory(${pybind11_DIR} ${CMAKE_CURRENT_BINARY_DIR}/pybind11-build)
else()
	# if pybind11_DIR is not provided, download pybind11 from its github home
	FetchContent_Declare(
		pybind11
		GIT_REPOSITORY https://github.com/pybind/pybind11.git
		GIT_TAG "v${PYBIND11_VERSION}"
		GIT_SUBMODULES ""
	)
	FetchContent_GetProperties(pybind11)
	if(NOT pybind11_POPULATED)
		FetchContent_Populate(pybind11)
		add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
	endif()
endif()
