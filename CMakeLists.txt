cmake_minimum_required(VERSION 3.29)
project(OpenGlTests)

set(CMAKE_CXX_STANDARD 17)

# GLEW
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.1.0)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# GLFW
add_subdirectory(libs/glfw)

add_executable(OpenGlTests main.cpp)

target_link_libraries(OpenGlTests glfw ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
target_include_directories(OpenGlTests PRIVATE ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} libs/glm)