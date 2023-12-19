#
# PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
#
# Copyright (c) 2012-2023 Esri R&D Center Zurich
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


### python bindings dependency

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
set(CMAKE_CXX_FLAGS_MINSIZEREL "")
set(CMAKE_CXX_FLAGS_DEBUG "")

message(STATUS "Python Module Suffix: " ${PYTHON_MODULE_EXTENSION})

pybind11_add_module(${CLIENT_TARGET} MODULE
		utils.cpp
		api.cpp
		PyCallbacks.cpp
		PRTContext.cpp
		PythonLogHandler.cpp
		InitialShape.cpp
		GeneratedModel.cpp
		ModelGenerator.cpp)

set_target_properties(${CLIENT_TARGET} PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		CXX_STANDARD_REQUIRED TRUE)

if(PYPRT_WINDOWS)
	target_compile_options(${CLIENT_TARGET} PRIVATE
			-GL -W4 -MD -permissive-
			-bigobj -GR -EHsc
			$<$<CONFIG:RELWITHDEBINFO>:-Od -Zi>
			$<$<CONFIG:RELEASE>:-O2>)
	target_compile_definitions(${CODEC_TARGET} PRIVATE
			-DNDEBUG -DWIN32 -D_WINDOWS)

elseif(PYPRT_LINUX)
	target_compile_options(${CLIENT_TARGET} PRIVATE
			-march=nocona
			-fvisibility=hidden -fvisibility-inlines-hidden
			-Wl,--exclude-libs,ALL
			-Wall -Wextra -Wunused-parameter
			$<$<CONFIG:RELWITHDEBINFO>:-O0>
			$<$<CONFIG:RELEASE>:-O3>)

	target_compile_definitions(${CLIENT_TARGET} PRIVATE
			-DNDEBUG
			-D_GLIBCXX_USE_CXX11_ABI=0) # we follow RHEL and use the pre-C++11 libstdc++ mode

	set_target_properties(${CLIENT_TARGET} PROPERTIES
			INSTALL_RPATH "\$ORIGIN"
			INSTALL_RPATH_USE_LINK_PATH FALSE
			SKIP_RPATH FALSE
			BUILD_WITH_INSTALL_RPATH TRUE)
endif()

target_link_libraries(${CLIENT_TARGET} PRIVATE
		${PRT_LINK_LIBRARIES})

target_include_directories(${CLIENT_TARGET} PRIVATE
     ${PRT_INCLUDE_PATH}
     ${PROJECT_SOURCE_DIR}/codec)

target_compile_definitions(${CLIENT_TARGET} PRIVATE
		-DPRT_VERSION_MAJOR=${PRT_VERSION_MAJOR}
		-DPRT_VERSION_MINOR=${PRT_VERSION_MINOR})


### install target

install(TARGETS ${CLIENT_TARGET} ARCHIVE DESTINATION lib LIBRARY DESTINATION bin)

# PRT puts import and shared libraries into PRT_LIBRARIES, let's filter out the import libraries
set(PYPRT_PRT_LIBRARIES ${PRT_LIBRARIES})
if (NOT "${CMAKE_IMPORT_LIBRARY_SUFFIX}" STREQUAL "")
	string(REPLACE "." "\\." ESCAPED_IMP_LIB_SUF "${CMAKE_IMPORT_LIBRARY_SUFFIX}")
	list(FILTER PYPRT_PRT_LIBRARIES EXCLUDE REGEX "${ESCAPED_IMP_LIB_SUF}")
endif()
install(FILES ${PYPRT_PRT_LIBRARIES} DESTINATION bin)

install(FILES ${PRT_EXT_LIBRARIES} DESTINATION lib)
install(DIRECTORY ${PRT_EXT_RESOURCES} DESTINATION lib MESSAGE_NEVER)
