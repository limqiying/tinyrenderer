cmake_minimum_required(VERSION 3.15.4)
project(tinyrenderer)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

file(GLOB SOURCES "src/*.cpp")
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
file(COPY obj DESTINATION ${CMAKE_BINARY_DIR})

add_executable(tinyrenderer ${SOURCES})
target_link_libraries (tinyrenderer Eigen3::Eigen)