cmake_minimum_required(VERSION 3.15)
project(raytracer C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -Wshadow -Wpedantic")

# Create core library
add_library(core
  core/color.c
  core/dyn_array.c
  core/scene_parser.c
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create hittable library
add_library(hittable
  hittable/hittable.c
  hittable/hittable_list.c
  hittable/sphere.c
  hittable/plane.c
  hittable/triangle.c
  hittable/triangle_mesh.c
)
target_include_directories(hittable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# # Create material library
# add_library(material
# )
# target_include_directories(material PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# Create main executable
add_executable(raytracer main.c camera.c)

# Link everything properly
target_link_libraries(raytracer core hittable m)
