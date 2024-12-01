cmake_minimum_required(VERSION 3.15)
project(vtnibbler)

set(
    MAIN_FILES
    "src/main.cpp"
    "src/capabilities.cpp"
    "src/coloring.cpp"
    "src/engine.cpp"
    "src/font.cpp"
    "src/levels.cpp"
    "src/options.cpp"
    "src/os.cpp"
    "src/screen.cpp"
    "src/snake.cpp"
    "src/status.cpp"
)

set(
    DOC_FILES
    "README.md"
    "LICENSE.txt"
)

if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

add_executable(vtnibbler ${MAIN_FILES})

if(UNIX)
    target_link_libraries(vtnibbler -lpthread)
endif()

set_target_properties(vtnibbler PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED On)
source_group("Doc Files" FILES ${DOC_FILES})
