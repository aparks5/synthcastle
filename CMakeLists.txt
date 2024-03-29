﻿# CMakeList.txt : CMake project for synthcastle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
# ...set this right below the cmake_minimum_required
set(CMAKE_TOOLCHAIN_FILE ./vcpkg/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET x64-windows CACHE STRING "")

#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE Release)
#endif()

# msvc optimize for speed, use intrinsics, omit frame pointer
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Oi /Oy")

#add_compile_definitions(-D PA_USE_ASIO 1)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("synthcastle")

file(GLOB source
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c")

include_directories(
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR}/include/moog
    ${PROJECT_SOURCE_DIR}/include/thirdparty
    ${PROJECT_SOURCE_DIR}/src)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${source})
 
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

include_directories(./vcpkg/installed/x64-windows/include)
link_directories(./vcpkg/installed/x64-windows/lib)

if(MSVC)
    add_compile_definitions(SDL_MAIN_HANDLED)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

find_package(PThreads4W REQUIRED)
find_package(portaudio CONFIG REQUIRED)
find_package(rtmidi CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fftw3 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)

# ImGuiFileDialog
add_library(ImGuiFileDialog)
target_sources(ImGuiFileDialog PRIVATE
    ${PROJECT_SOURCE_DIR}/include/thirdparty/dirent/dirent.h
    ${PROJECT_SOURCE_DIR}/include/thirdparty/ImGuiFileDialog.h
    ${PROJECT_SOURCE_DIR}/include/thirdparty/ImGuiFileDialogConfig.h
    ${PROJECT_SOURCE_DIR}/src/ImGuiFileDialog.cpp)
target_include_directories(ImGuiFileDialog PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(ImGuiFileDialog PUBLIC imgui::imgui)

# Imnodes
add_library(imnodes)
target_sources(imnodes PRIVATE
    ${PROJECT_SOURCE_DIR}/include/imnodes.h
    ${PROJECT_SOURCE_DIR}/include/imnodes_internal.h
    ${PROJECT_SOURCE_DIR}/src/imnodes.cpp)
target_include_directories(imnodes PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(imnodes PUBLIC imgui::imgui)

find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

# vcpkg bootstrap and install (all should have -x64-windows appended)
# portaudio
# opengl, pthreads, rtmidi, spdlog, fftw3, glfw3, glad, imgui, implot, glfw3, 
# sdl2
# imgui[glfw-binding,opengl32-binding,sdl2-binding]

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    "opengl32"
    PThreads4W::PThreads4W
    portaudio
    RtMidi::rtmidi
    spdlog::spdlog
    FFTW3::fftw3
    glfw
    glad::glad
    imgui::imgui
	implot::implot)
