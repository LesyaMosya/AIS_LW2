cmake_minimum_required(VERSION 3.22)
project(AIS_LW2_Server)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

add_executable(AIS_LW2_Server main.cpp
        TextException.h
        Server.cpp
        Server.h
        ConcurentQueue.h
)

target_link_libraries(AIS_LW2_Server ${libname}++)