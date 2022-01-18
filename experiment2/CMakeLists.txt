cmake_minimum_required(VERSION 3.1.0)

project(exp2)

# Set to c++11
set(CMAKE_CXX_STANDARD 11)

# Source
set(HEADERS
        code/face.h
        code/color.h
        code/camera.h
        code/hittable.h
        code/hittable_list.h
        code/material.h
        code/rtweekend.h
        code/sphere.h
        code/vec3.h
        code/aabb.h
        code/objectReader.h)

set(SOURCES
        code/main.cc)


# Executables
add_executable(exp2 ${HEADERS} ${SOURCES})
