file(GLOB SRC "*.cpp")
add_library(util STATIC ${SRC})
target_include_directories(util PUBLIC ${CMAKE_SOURCE_DIR}/src/util)
