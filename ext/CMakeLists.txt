cmake_minimum_required(VERSION 3.15)
project(NFD)

include_directories(nfd/src nfd/src/include)

if(UNIX)
add_library(NFD nfd/src/nfd_common.c nfd/src/nfd_zenity.c)
endif(UNIX)
if(MSVC)
add_library(NFD nfd/src/nfd_common.c nfd/src/nfd_win.cpp)
target_link_libraries(NFD comctl32)
endif(MSVC)
