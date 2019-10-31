cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)


### setup build target

add_library(${CODEC_TARGET} SHARED
		codecs_Py.cpp
		encoder/PyEncoder.cpp)

target_compile_features(${CODEC_TARGET} PRIVATE
		cxx_std_14) # C++14

target_compile_definitions(${CODEC_TARGET} PRIVATE
		-DPRT_VERSION_MAJOR=${PRT_VERSION_MAJOR}
		-DPRT_VERSION_MINOR=${PRT_VERSION_MINOR})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_compile_options(${CODEC_TARGET} PRIVATE
			-bigobj -GR -EHsc)

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_compile_options(${CODEC_TARGET} PRIVATE
			-D_GLIBCXX_USE_CXX11_ABI=0 # we follow RHEL and use the pre-C++11 libstdc++ mode
			-march=nocona
			-fvisibility=hidden -fvisibility-inlines-hidden
			-Wl,--exclude-libs,ALL
			-Wall -Wextra -Wunused-parameter)

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_compile_options(${CODEC_TARGET} PRIVATE
			-stdlib=libc++
			-fvisibility=hidden -fvisibility-inlines-hidden
			-Wl,--exclude-libs,ALL
			-Wall -Wextra -Wunused-parameter)
endif()


### add prt dependency

target_link_libraries(${CODEC_TARGET} PRIVATE ${PRT_LINK_LIBRARIES})
target_include_directories(${CODEC_TARGET} PRIVATE ${PRT_INCLUDE_PATH})


### install target

install(TARGETS ${CODEC_TARGET} RUNTIME DESTINATION lib LIBRARY DESTINATION lib)
