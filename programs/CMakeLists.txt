add_executable(rutok
  ${CMAKE_CURRENT_SOURCE_DIR}/tokenizer_bin/main.cpp)

add_executable(sentizer
  ${CMAKE_CURRENT_SOURCE_DIR}/sentizer_bin/main.cpp)

target_link_libraries(rutok rutoklib)
target_link_libraries(sentizer rutoklib)
add_sanitizers(rutok)
add_sanitizers(sentizer)
