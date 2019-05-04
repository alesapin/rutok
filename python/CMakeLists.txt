pybind11_add_module(rutokpy rutok.cpp)
target_link_libraries(rutokpy PRIVATE rutoklib)
