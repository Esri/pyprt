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

# if prt_DIR is not provided, download PRT from its github home
if(NOT prt_DIR)
	if(PYPRT_WINDOWS)
		set(PRT_OS "win10")
		set(PRT_TC "vc142")
	elseif(PYPRT_LINUX)
		set(PRT_OS "rhel7")
		set(PRT_TC "gcc63")
	elseif(PYPRT_MACOS)
		set(PRT_OS "osx12")
		set(PRT_TC "ac81")
	endif()

	set(PRT_VERSION "2.2.6332")
	set(PRT_ARCHIVE "esri_ce_sdk-${PRT_VERSION}-${PRT_OS}-${PRT_TC}-x86_64-rel-opt.zip")
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
