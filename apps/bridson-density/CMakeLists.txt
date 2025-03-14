file(GLOB SRC "*.cpp")
add_executable(BridsonDensity ${SRC})
target_link_libraries(BridsonDensity PRIVATE application ${LIBRARIES})
target_include_directories(BridsonDensity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
