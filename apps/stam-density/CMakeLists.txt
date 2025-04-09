file(GLOB SRC "*.cpp")
add_executable(StamDensity ${SRC})
target_link_libraries(StamDensity PRIVATE application ${LIBRARIES})
target_include_directories(StamDensity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
