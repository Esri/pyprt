cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)


### build target

add_library(${CODEC_TARGET} SHARED codecs_Py.cpp encoder/PyEncoder.cpp)
target_compile_definitions(${CODEC_TARGET} PRIVATE -DPRT_VERSION_MAJOR=${PRT_VERSION_MAJOR} -DPRT_VERSION_MINOR=${PRT_VERSION_MINOR})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_compile_options(${CODEC_TARGET} PRIVATE -std=c++11 -march=nocona -fvisibility=hidden -fvisibility-inlines-hidden -Wl,--exclude-libs,ALL -Wall -Wextra -Wunused-parameter -Wl,-rpath,'$ORIGIN')
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_compile_options(${CODEC_TARGET} PRIVATE -std=c++14 -stdlib=libc++ -fvisibility=hidden -fvisibility-inlines-hidden -Wl,--exclude-libs,ALL -Wall -Wextra -Wunused-parameter)
endif()


### add prt dependency

target_link_libraries(${CODEC_TARGET} PRIVATE ${PRT_LINK_LIBRARIES})
target_include_directories(${CODEC_TARGET} PRIVATE ${PRT_INCLUDE_PATH})

### install target

install(TARGETS ${CODEC_TARGET} RUNTIME DESTINATION lib LIBRARY DESTINATION lib)
