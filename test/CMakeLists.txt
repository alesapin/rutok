add_executable(testrun
  ${CMAKE_CURRENT_SOURCE_DIR}/unittest/token_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unittest/token_stream_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unittest/sentence_test.cpp
)

target_include_directories(testrun PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(testrun gtest_main rutoklib)
add_sanitizers(testrun)
