cmake_minimum_required(VERSION 3.19)
project(quizzer)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_GetProperties(raylib)
if (NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif()
add_executable(${PROJECT_NAME} "./qz_main.cpp")
target_link_libraries(${PROJECT_NAME} raylib)
file(COPY "./data" DESTINATION ".")
configure_file("./data/RobotoMono-Regular.ttf" "./data/RobotoMono-Regular.ttf" COPYONLY)
configure_file("./data/quiz.txt" "./data/quiz.txt" COPYONLY)
