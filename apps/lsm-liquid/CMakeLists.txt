file(GLOB SRC "*.cpp")
add_executable(LSMLiquid ${SRC})
target_link_libraries(LSMLiquid PRIVATE application ${LIBRARIES})
target_include_directories(LSMLiquid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
