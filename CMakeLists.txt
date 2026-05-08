cmake_minimum_required(VERSION 3.11)

include(FetchContent)
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
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LOGL_ROOT "${CMAKE_BINARY_DIR}")
if(GROUPED_LIBS)
    cmake_print_variables(${GROUPED_LIBS})
    set(GLAD_LIB ${EXTERNAL_LIBS}/glad)
    set(GLFW_LIB ${EXTERNAL_LIBS}/glfw)    
    cmake_print_variables(${GLAD_LIB} ${GLFW_LIB})
endif(GROUPED_LIBS)

file(GLOB LIB_SOURCES "${CMAKE_SOURCE_DIR}/lib/*.cpp")
file(GLOB SRC_FILES "src/*.cpp")

add_subdirectory ( ${GLFW_LIB} mbuild_glfw)
find_package(OpenGL REQUIRED)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	8d1fd52 #refs/tags/1.0.3
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY	https://github.com/assimp/assimp.git
	GIT_TAG 	e0b5234  #refs/tags/6.0.4 
)

FetchContent_MakeAvailable(assimp)


FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)


add_executable(${PROJECT_NAME} ${SRC_FILES} ${EXTERNAL_LIBS}/glad/src/glad.c ${LIB_SOURCES})

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
        glm::glm
        assimp
        nlohmann_json::nlohmann_json
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${GLAD_LIB}/include
        ${GLFW_LIB}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cmake
        ${CMAKE_BINARY_DIR}/generated
)
