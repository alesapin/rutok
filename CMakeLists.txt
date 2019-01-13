cmake_minimum_required(VERSION 3.8.0)
project(rutok LANGUAGES CXX)

set(CMAKE_CONTRIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/contrib/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

SET(UNICORN_BUILD_TESTING 0)
add_subdirectory(${CMAKE_CONTRIB_DIR}/unicorn-lib)

option(INSTALL_GTEST OFF)
option(BUILD_GMOCK OFF)
add_subdirectory(${CMAKE_CONTRIB_DIR}/googletest)
add_subdirectory(${CMAKE_CONTRIB_DIR}/span-lite)
add_subdirectory(${CMAKE_CONTRIB_DIR}/libcharsetdetect)

option(RAPIDJSON_BUILD_DOC OFF)
option(RAPIDJSON_BUILD_TESTS OFF)
option(RAPIDJSON_BUILD_EXAMPLES OFF)
add_subdirectory(${CMAKE_CONTRIB_DIR}/rapidjson)

SET(CMAKE_C_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
find_package(Sanitizers)

add_library(rutoklib
  src/core/charProperties.cpp
  src/core/Tag.cpp
  src/core/Token.cpp
  src/core/Sentence.cpp
  src/streams/TokenInputStream.cpp
  src/streams/TokenStringOutputStream.cpp
  src/streams/TokenJSONOutputStream.cpp
  src/streams/BaseTokenConcatInputStream.cpp
  src/streams/SmallGroupsTokenConcatInputStream.cpp
  src/streams/SentenceInputStream.cpp
  src/streams/EncodingInputStream.cpp
  src/streams/EncodingOutputStream.cpp
)

target_compile_features(rutoklib PUBLIC cxx_std_17)
target_link_libraries(rutoklib RS::Unicorn)
target_link_libraries(rutoklib span-lite)
target_link_libraries(rutoklib charsetdetect)
target_include_directories(rutoklib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(rutoklib PUBLIC ${CMAKE_CONTRIB_DIR}/span-lite/include)
target_include_directories(rutoklib PUBLIC ${CMAKE_CONTRIB_DIR}/rapidjson/include)
target_include_directories(rutoklib PUBLIC ${CMAKE_CONTRIB_DIR}/libcharsetdetect)

add_executable(rutok
  ${CMAKE_CURRENT_SOURCE_DIR}/programs/tokenizer/main.cpp)

target_link_libraries(rutok rutoklib)
add_sanitizers(rutok)

add_executable(testrun
  ${CMAKE_CURRENT_SOURCE_DIR}/test/token_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/token_stream_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/sentence_test.cpp
)


target_include_directories(testrun PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(testrun gtest_main rutoklib)
add_sanitizers(testrun)
