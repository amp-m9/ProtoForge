cmake_minimum_required(VERSION 3.10)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)
set(EXTERNAL_LIBS "${CMAKE_SOURCE_DIR}/../Libs")
set(DCMAKE_EXPORT_COMPILE_COMMANDS, ON)
set(LOGL_ROOT "${CMAKE_BINARY_DIR}")
file(GLOB LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/lib/*.cpp")

add_subdirectory(../Libs/glfw-3.4 build_glfw)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp ${EXTERNAL_LIBS}/glad/src/glad.c ${LIB_SOURCES})

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/root_directory.h.in
    ${CMAKE_SOURCE_DIR}/cmake/root_directory.h
    @ONLY
)

include_directories(${CMAKE_BINARY_DIR}/generated)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        OpenGL::GL
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${EXTERNAL_LIBS}/glad/include       # <-- path to glad headers
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/cmake

)
