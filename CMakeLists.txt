cmake_minimum_required(VERSION 3.8.0)
project(rutok LANGUAGES CXX)

set(CMAKE_CONTRIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/contrib)

add_subdirectory(${CMAKE_CONTRIB_DIR}/unicorn-lib)
add_subdirectory(${CMAKE_CONTRIB_DIR}/googletest)

add_library(rutoklib
  src/core/Token.cpp
)

target_compile_features(rutoklib PUBLIC cxx_std_17)
target_link_libraries(rutoklib RS::Unicorn)

add_executable(rutok
  ${CMAKE_CURRENT_SOURCE_DIR}/programs/tokenizer/main.cpp)

target_link_libraries(rutok rutoklib)

add_executable(token_test
 ${CMAKE_CURRENT_SOURCE_DIR}/test/token_test.cpp)

target_include_directories(token_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(token_test gtest_main rutoklib)
