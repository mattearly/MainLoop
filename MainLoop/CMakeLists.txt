

add_library(MainLoop MainLoop.cpp pch.cpp framework.h ../include/MainLoop.h pch.h)

target_include_directories(MainLoop PRIVATE ../include/)
