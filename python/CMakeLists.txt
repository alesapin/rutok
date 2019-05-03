pybind11_add_module(pyrutok rutok.cpp)
target_link_libraries(pyrutok PRIVATE rutoklib)
