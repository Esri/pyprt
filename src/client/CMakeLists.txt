cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)


### python bindings dependency

message(STATUS "Python Module Suffix: " ${PYTHON_MODULE_EXTENSION})

pybind11_add_module(${CLIENT_TARGET} MODULE
		utils.cpp
		wrap.cpp
		PyCallbacks.cpp)

target_compile_features(${CODEC_TARGET} PRIVATE
		cxx_std_14) # C++14

if(PYPRT_WINDOWS)
	# TODO

elseif(PYPRT_LINUX)
	target_compile_options(${CLIENT_TARGET} PRIVATE
			-D_GLIBCXX_USE_CXX11_ABI=0
			-march=nocona
			-fvisibility=hidden -fvisibility-inlines-hidden
			-Wl,--exclude-libs,ALL
			-Wall -Wextra -Wunused-parameter)

	set_target_properties(${PROJECT_NAME} PROPERTIES
			INSTALL_RPATH "\$ORIGIN"
			INSTALL_RPATH_USE_LINK_PATH FALSE
			SKIP_RPATH FALSE
			BUILD_WITH_INSTALL_RPATH TRUE)
		
elseif(PYPRT_MACOS)
	# TODO

endif()

target_link_libraries(${CLIENT_TARGET} PRIVATE
		${PRT_LINK_LIBRARIES})

target_include_directories(${CLIENT_TARGET} PRIVATE
     ${PRT_INCLUDE_PATH}
     ${PROJECT_SOURCE_DIR}/codec/encoder)


### install target

install(TARGETS ${CLIENT_TARGET} ARCHIVE DESTINATION lib LIBRARY DESTINATION bin)
install(FILES ${PRT_LIBRARIES} DESTINATION bin OPTIONAL)
install(FILES ${PRT_EXT_LIBRARIES} DESTINATION lib)
