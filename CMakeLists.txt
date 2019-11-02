cmake_minimum_required(VERSION 3.15.4)
project(tinyrenderer)

include_directories(include)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

file(GLOB SOURCES "src/*.cpp")
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

add_executable(tinyrenderer ${SOURCES})
target_link_libraries (tinyrenderer Eigen3::Eigen)