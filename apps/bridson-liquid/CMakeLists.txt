file(GLOB SRC "*.cpp")
add_executable(BridsonLiquid ${SRC})
target_link_libraries(BridsonLiquid PRIVATE application ${LIBRARIES})
target_include_directories(BridsonLiquid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
