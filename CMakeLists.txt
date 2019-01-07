cmake_minimum_required(VERSION 3.8.0)
project(rutok LANGUAGES CXX)

set(CMAKE_CONTRIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/contrib)

add_subdirectory(${CMAKE_CONTRIB_DIR}/unicorn-lib)
add_subdirectory(${CMAKE_CONTRIB_DIR}/googletest)
add_subdirectory(${CMAKE_CONTRIB_DIR}/span-lite)
add_subdirectory(${CMAKE_CONTRIB_DIR}/rapidjson)

add_library(rutoklib
  src/core/charProperties.cpp
  src/core/Tag.cpp
  src/core/Token.cpp
  src/streams/TokenInputStream.cpp
  src/streams/TokenStringOutputStream.cpp
  src/streams/TokenJSONOutputStream.cpp
  src/streams/BaseTokenConcatInputStream.cpp
  src/streams/SmallGroupsTokenConcatInputStream.cpp
)

target_compile_features(rutoklib PUBLIC cxx_std_17)
target_link_libraries(rutoklib RS::Unicorn)
target_link_libraries(rutoklib span-lite)
target_include_directories(rutoklib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(rutoklib PUBLIC ${CMAKE_CONTRIB_DIR}/span-lite/include)
target_include_directories(rutoklib PUBLIC ${CMAKE_CONTRIB_DIR}/rapidjson/include)

add_executable(rutok
  ${CMAKE_CURRENT_SOURCE_DIR}/programs/tokenizer/main.cpp)

target_link_libraries(rutok rutoklib)

add_executable(token_test
  ${CMAKE_CURRENT_SOURCE_DIR}/test/token_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/token_stream_test.cpp
)


target_include_directories(token_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(token_test gtest_main rutoklib)