cmake_minimum_required(VERSION 3.26)
project(StarburstRemake)

set(CMAKE_CXX_STANDARD 20)

# Replace the paths below with the actual paths from your MSYS2 installation
set(YAML_CPP_INCLUDE_DIR "C:/msys64/mingw64/include")
set(YAML_CPP_LIBRARIES "C:/msys64/mingw64/lib/libyaml-cpp.dll.a") # or libyaml-cpp.so for shared lib

include_directories(${YAML_CPP_INCLUDE_DIR})
link_directories(${YAML_CPP_LIBRARIES})

# For each target that uses yaml-cpp, link it against the library
add_executable(StarburstRemake main.cpp
        config.cpp
        config.h
       )

target_link_libraries(StarburstRemake ${YAML_CPP_LIBRARIES})