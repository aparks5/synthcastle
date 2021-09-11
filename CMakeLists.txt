cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("synthcastle")

file(GLOB source
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/include/moog/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp")

include_directories(
    ${PROJECT_SOURCE_DIR}/include 
	${PROJECT_SOURCE_DIR}/include/moog
    ${PROJECT_SOURCE_DIR}/src)

add_executable (${PROJECT_NAME} ${source} "include/delay.h")

# PortAudio
add_subdirectory("C:/portaudio" "C:/portaudio/out" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} portaudio_static)

# RtMidi
add_subdirectory("C:/rtmidi " "C:/rtmidi/out" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} rtmidi)



# GLFW (OpenGL)
set(GLFW_DIR "C:/GL/glfw")
add_subdirectory("${GLFW_DIR}" "${GLFW_DIR}/out" EXCLUDE_FROM_ALL)
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/deps")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")


# glad (OpenGL)
set(GLAD_DIR "C:/GL/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c" "include/delay.h")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")




