file(GLOB SRC "*.cpp")
add_library(math STATIC ${SRC})
target_include_directories(math PUBLIC ${CMAKE_SOURCE_DIR}/src/math)
target_link_libraries(math PUBLIC util ${LIBRARIES})
