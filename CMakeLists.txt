cmake_minimum_required(VERSION 3.15)

project(3DH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

include_directories("./ext/nfd/src/include" "./inc" "./MARE/inc" "./MARE/ext/glfw/include" "./MARE/ext/glew-2.1.0/include" "./MARE/ext/glm/glm" "./MARE/ext/loaders/stb")

set(BUILD_SHARED_LIBS OFF)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

add_subdirectory(./MARE)
add_subdirectory(./ext)

set(SRC
./src/main.cpp)

add_executable(3DH ${SRC})

target_link_libraries(3DH MARE NFD)