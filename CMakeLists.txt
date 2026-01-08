cmake_minimum_required(VERSION 3.10)
include(CMakePrintHelpers)
project(LearnOpenGL)

file(STRINGS CMakeVariables.txt ConfigContents)
foreach(NameAndValue ${ConfigContents})
  # Strip leading spaces
  string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
  # Find variable name
  string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
  # Find the value
  string(REPLACE "${Name}=" "" Value ${NameAndValue})
  # Set the variable
  set(${Name} "${Value}")
  cmake_print_variables(${Name})
endforeach()

set(CMAKE_CXX_STANDARD 17)

set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LOGL_ROOT "${CMAKE_BINARY_DIR}")
file(GLOB LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/lib/*.cpp")

add_subdirectory(../../Libs/glfw-3.4 build_glfw)
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

if(GROUPED_LIBS)
    cmake_print_variables(${GROUPED_LIBS})
    set(GLAD_LIB ${EXTERNAL_LIBS}/glad/include)
    set(GLFW_LIB ${EXTERNAL_LIBS}/glfw-3.4/include)    
    cmake_print_variables(${GLAD_LIB} ${GLFW_LIB})
endif(GROUPED_LIBS)

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${GLAD_LIB}
        ${GLFW_LIB}  
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/cmake
        ${CMAKE_BINARY_DIR}/generated
)
