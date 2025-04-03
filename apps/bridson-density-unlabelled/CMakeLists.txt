file(GLOB SRC "*.cpp")
add_executable(BridsonDensityUnlabelled ${SRC})
target_link_libraries(BridsonDensityUnlabelled PRIVATE application ${LIBRARIES})
target_include_directories(BridsonDensityUnlabelled PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
