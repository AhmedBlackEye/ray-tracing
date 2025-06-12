cmake_minimum_required(VERSION 3.15)
project(raytracer C)

set(CMAKE_C_STANDARD 11)

# Create core library
add_library(core
  core/color.c
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create hittable library
add_library(hittable
  hittable/sphere.c
)
target_include_directories(hittable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create main executable
add_executable(raytracer main.c)

# Link everything properly
target_link_libraries(raytracer core hittable m)
