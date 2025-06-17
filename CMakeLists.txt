cmake_minimum_required(VERSION 3.15)
project(raytracer C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -Wshadow -Wpedantic")

# Create core library
add_library(core
  core/color.c
  core/dyn_array.c
  core/scene_parser.c
  core/aabb.c
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create hittable library
add_library(hittable
  hittable/hittable.c
  hittable/hit_record.c
  hittable/hittable_list.c
  hittable/sphere.c
  hittable/plane.c
  hittable/triangle.c
  hittable/triangle_mesh.c
  hittable/quad.c
  hittable/bvh_node.c
)
target_include_directories(hittable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hittable PUBLIC core)

# Create material library
add_library(material
material/material.c
material/lambertian.c
material/metal.c
material/dielectric.c
material/diffuse_light.c
)
target_include_directories(material PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create texture library
add_library(texture
texture/texture.c
texture/checkered.c
texture/solid_color.c
)
target_include_directories(texture PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# Create main executable
add_executable(raytracer main.c camera.c shared.c scene.c)

# Link everything properly
target_link_libraries(raytracer core hittable material texture m)
