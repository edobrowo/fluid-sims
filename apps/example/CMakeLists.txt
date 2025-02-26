file(GLOB SRC "*.cpp")
add_executable(Example ${SRC})
target_link_libraries(Example PRIVATE application ${LIBRARIES})
target_include_directories(Example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
