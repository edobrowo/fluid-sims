file(GLOB SRC "*.cpp")
add_executable(BridsonMullerFischer ${SRC})
target_link_libraries(BridsonMullerFischer PRIVATE application ${LIBRARIES})
target_include_directories(BridsonMullerFischer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
