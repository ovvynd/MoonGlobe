cmake_minimum_required(VERSION 2.8.12)

project(m2-sphere)

get_filename_component(M2_ROOT "${CMAKE_SOURCE_DIR}" ABSOLUTE)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(M2_DEBUG ON)
    set(M2_BIN_DIR "${M2_ROOT}/bin/Debug")
    set(M2_LIB_DIR "${M2_ROOT}/bin/Debug/libs")
    set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(M2_RELEASE ON)
    set(M2_BIN_DIR "${M2_ROOT}/bin/Release")
    set(M2_LIB_DIR "${M2_ROOT}/bin/Release/libs")
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE value (Debug|Release)")
endif()

message(STATUS "binaries output: " ${M2_BIN_DIR})
message(STATUS "libraries output: " ${M2_LIB_DIR})

# ---------------------------  paths  ---------------------------
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY_DEBUG   ${M2_BIN_DIR})
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY_RELEASE ${M2_BIN_DIR})
set(CMAKE_EXECUTABLE_OUTPUT_PATH              ${M2_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG      ${M2_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE    ${M2_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_PATH                 ${M2_LIB_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG      ${M2_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE    ${M2_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY            ${M2_BIN_DIR})

#----------------------- compile options ------------------------
set(CMAKE_CXX_STANDARD 14)


#----------------------- add libs ------------------------

#module_path for magnum, magnum-plugins, corrade
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/libs/modules/")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin/libs)

# lib components
set(WITH_SHADERS         ON)
set(WITH_MESHTOOLS       ON)
set(WITH_PRIMITIVES      ON)
set(WITH_TGAIMPORTER     ON)
set(WITH_SDL2APPLICATION ON)
set(WITH_JPEGIMPORTER    ON)
set(WITH_PNGIMPORTER     ON)

# set target mobile version OpenGL
set(TARGET_GLES ON)

add_subdirectory(libs/corrade)
add_subdirectory(libs/magnum)
add_subdirectory(libs/magnum-plugins)

find_package(Magnum REQUIRED
        Primitives
        MeshTools
        Shaders
        Sdl2Application)

#wtf?
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(MAGNUM_PLUGINS_IMPORTER_DIR "${M2_LIB_DIR}")

set(LIBS ${LIBS}
        Magnum
        Magnum::Application
        Magnum::Primitives
        Magnum::Shaders
        Magnum::MeshTools
        Magnum::Magnum)

#----------------------- apply configurations files ------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/configure.h.cmake
               ${CMAKE_CURRENT_SOURCE_DIR}/src/configure.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

#----------------------- add resources ------------------------
corrade_add_resource(RESOURCES resources/resources.conf)

#----------------------- sources ------------------------
set(SOURCES
        src/main.cpp
        src/moonglobe.cpp)

#----------------------- headers ---------------------------
include_directories(libs/magnum)
include_directories(src)


#----------------------- add target ------------------------
add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

#----------------------- link libraries to target ------------------------
target_link_libraries(${PROJECT_NAME} ${LIBS})
