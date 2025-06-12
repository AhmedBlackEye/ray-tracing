cmake_minimum_required(VERSION 3.15)
project(raytracer C)

set(CMAKE_C_STANDARD 11)

include_directories(core hittable)

add_executable(raytracer
  main.c
  core/color.c
  hittable/sphere.c
)

target_link_libraries(raytracer m)
