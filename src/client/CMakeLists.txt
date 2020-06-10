# Copyright (c) 2012-2020 Esri R&D Center Zurich
 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
 
#   http://www.apache.org/licenses/LICENSE-2.0
 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.

cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)


### python bindings dependency

message(STATUS "Python Module Suffix: " ${PYTHON_MODULE_EXTENSION})

pybind11_add_module(${CLIENT_TARGET} MODULE
		utils.cpp
		wrap.cpp
		PyCallbacks.cpp)

set_target_properties(${CLIENT_TARGET}
		PROPERTIES CXX_STANDARD 17
		CXX_STANDARD_REQUIRED TRUE)

if(PYPRT_WINDOWS)
	# nothing required

elseif(PYPRT_LINUX)
	target_compile_options(${CLIENT_TARGET} PRIVATE
			-D_GLIBCXX_USE_CXX11_ABI=0
			-march=nocona
			-fvisibility=hidden -fvisibility-inlines-hidden
			-Wl,--exclude-libs,ALL
			-Wall -Wextra -Wunused-parameter)

	set_target_properties(${CLIENT_TARGET} PROPERTIES
			INSTALL_RPATH "\$ORIGIN"
			INSTALL_RPATH_USE_LINK_PATH FALSE
			SKIP_RPATH FALSE
			BUILD_WITH_INSTALL_RPATH TRUE)

	# GCC 8 needs explicit linking of C++17 std::filesystem lib
	target_link_libraries(${CLIENT_TARGET} PRIVATE stdc++fs)

elseif(PYPRT_MACOS)
	target_compile_options(${CLIENT_TARGET} PRIVATE
			-stdlib=libc++
			-fvisibility=hidden -fvisibility-inlines-hidden
			-Wall -Wextra -Wunused-parameter
			-mmacosx-version-min=10.15) # required by C++17

	set_target_properties(${CLIENT_TARGET} PROPERTIES
			INSTALL_RPATH "@loader_path"
			INSTALL_RPATH_USE_LINK_PATH FALSE
			SKIP_RPATH FALSE
			BUILD_WITH_INSTALL_RPATH TRUE)

endif()

target_link_libraries(${CLIENT_TARGET} PRIVATE
		${PRT_LINK_LIBRARIES})

target_include_directories(${CLIENT_TARGET} PRIVATE
     ${PRT_INCLUDE_PATH}
     ${PROJECT_SOURCE_DIR}/codec)


### install target

install(TARGETS ${CLIENT_TARGET} ARCHIVE DESTINATION lib LIBRARY DESTINATION bin)
install(FILES ${PRT_LIBRARIES} DESTINATION bin OPTIONAL)
install(FILES ${PRT_EXT_LIBRARIES} DESTINATION lib)
install(DIRECTORY ${PRT_EXT_RESOURCES} DESTINATION lib MESSAGE_NEVER)
