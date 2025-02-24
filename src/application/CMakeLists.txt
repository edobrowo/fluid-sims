file(GLOB SRC "*.cpp")
add_library(application STATIC ${SRC})
target_include_directories(application PUBLIC ${CMAKE_SOURCE_DIR}/src/application)
target_link_libraries(application PUBLIC util math platform ${LIBRARIES})
