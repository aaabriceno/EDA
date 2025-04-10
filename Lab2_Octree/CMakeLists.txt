cmake_minimum_required(VERSION 3.10)

project(MiProyecto)

set(CMAKE_CXX_STANDARD 17)

# Configuración de VTK
set(VTK_DIR "C:/Users/PC/VTK/build/lib/cmake/vtk-9.4")

find_package(VTK REQUIRED)
include_directories(${VTK_INCLUDE_DIRS})
link_directories(${VTK_LIBRARY_DIRS})

# Añadir el archivo fuente
add_executable(MiProyecto a.cpp)

# Vincular VTK
target_link_libraries(MiProyecto ${VTK_LIBRARIES})
