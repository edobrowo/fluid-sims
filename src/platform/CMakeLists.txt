file(GLOB SRC "*.cpp")
add_library(platform STATIC ${SRC})
target_include_directories(platform PUBLIC ${CMAKE_SOURCE_DIR}/src/platform)
target_link_libraries(platform PUBLIC util ${LIBRARIES})
