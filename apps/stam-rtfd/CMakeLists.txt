file(GLOB SRC "*.cpp")
add_executable(RealTimeFluidDynamicsJosStam ${SRC})
target_link_libraries(RealTimeFluidDynamicsJosStam PRIVATE application ${LIBRARIES})
target_include_directories(RealTimeFluidDynamicsJosStam PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
