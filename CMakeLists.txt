cmake_minimum_required(VERSION 3.20)

project(Salamander C)
set(CMAKE_C_STANDARD 11)

add_compile_definitions(SALAMANDER_DEBUG SALAMANDER_SCREEN_SPACE)

include_directories(lib/glfw/include lib/glad/include lib/cglm/include lib/stb)
link_directories(lib/glfw/lib)

add_executable(Salamander lib/glad/src/glad.c src/main.c src/glfw_platform.c src/opengl_renderer.c)
target_link_libraries(Salamander glfw3 opengl32)
