cmake_minimum_required(VERSION 3.15)

project(3DH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

find_package(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIRS})
include_directories("./HAZEN/inc" "./HAZEN/ext/Eigen" "./ext/nfd/src/include" "./inc" "./MARE/inc" "./MARE/ext/glfw/include" "./MARE/ext/glew-2.1.0/include" "./MARE/ext/glm/glm" "./MARE/ext/loaders/stb")
link_directories("C:/Program Files (x86)/Intel/oneAPI/mkl/2021.1.1/lib/intel64")

set(BUILD_SHARED_LIBS OFF)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

add_subdirectory(./MARE)
add_subdirectory(./HAZEN)
add_subdirectory(./ext)

set(SRC
./src/main.cpp
./src/HydraulicNetwork.cpp
./src/GDAL/gdal_io.cpp
./src/Math/math_3dh.cpp
./src/RibbonTools/LoadTool.cpp
./src/Terrain.cpp
./src/RibbonTools/NodeTool.cpp)

add_executable(3DH ${SRC})

target_link_libraries(3DH ${GDAL_LIBRARIES} MARE NFD HAZEN)