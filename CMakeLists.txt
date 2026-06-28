cmake_minimum_required(VERSION 3.20)
project(acronym-builder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# argh
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/argh)

add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} PUBLIC
  argh
)

target_compile_options(${PROJECT_NAME} PUBLIC -g)