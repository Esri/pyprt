cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0015 NEW)



### python bindings dependency

message(STATUS "Python Module System Information: " ${PYTHON_MODULE_EXTENSION})
set(PYTHON_MODULE_EXTENSION ".pyd")
pybind11_add_module(${CLIENT_TARGET} utils.cpp wrap.cpp)


target_link_libraries(${CLIENT_TARGET} PRIVATE ${PRT_LINK_LIBRARIES})
target_include_directories(${CLIENT_TARGET} PRIVATE ${PRT_INCLUDE_PATH})


### install target

install(TARGETS ${CLIENT_TARGET} ARCHIVE DESTINATION lib LIBRARY DESTINATION bin)
install(FILES ${PRT_LIBRARIES} DESTINATION bin OPTIONAL)
install(FILES ${PRT_EXT_LIBRARIES} DESTINATION lib)
