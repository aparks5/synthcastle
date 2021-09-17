cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("synthcastle")

file(GLOB source
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/include/moog/*.h"
	"${PROJECT_SOURCE_DIR}/include/thirdparty/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c")

include_directories(
    ${PROJECT_SOURCE_DIR}/include 
	${PROJECT_SOURCE_DIR}/include/moog
    ${PROJECT_SOURCE_DIR}/src)

add_executable (${PROJECT_NAME} ${source})

#### PortAudio
add_subdirectory("C:/portaudio" "C:/portaudio/out" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} portaudio_static)

#### RtMidi
add_subdirectory("C:/rtmidi " "C:/rtmidi/out" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} rtmidi)

#### GLFW (OpenGL)
set(GLFW_DIR "C:/GL/glfw")
add_subdirectory("${GLFW_DIR}" "${GLFW_DIR}/out" EXCLUDE_FROM_ALL)
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/deps")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")


#### glad (OpenGL)
set(GLAD_DIR "C:/GL/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")


#### google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#### unit tests
enable_testing()

file(GLOB tests 
	"${PROJECT_SOURCE_DIR}/src/tests/*.cpp")


add_executable(gtest_test ${tests})

target_link_libraries(
  gtest_test 
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(
 gtest_test
)



