cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("modern_dsp_framework")

file(GLOB source
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp")

include_directories(
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR}/src)

add_executable (modern_dsp_framework ${source})
add_subdirectory("C:/portaudio" "C:/portaudio/out" EXCLUDE_FROM_ALL)
target_link_libraries(modern_dsp_framework portaudio_static)

