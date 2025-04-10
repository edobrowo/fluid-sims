file(GLOB SRC "*.cpp")
add_executable(BridsonDensityLabelled ${SRC})
target_link_libraries(BridsonDensityLabelled PRIVATE application ${LIBRARIES})
target_include_directories(BridsonDensityLabelled PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
