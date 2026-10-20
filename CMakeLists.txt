cmake_minimum_required(VERSION 3.20)
project(wampAide VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(wampAide
    src/main.cpp
    src/cli_options.cpp
    src/config.cpp
    src/utils.cpp
)

target_include_directories(wampAide PRIVATE src)
target_compile_definitions(wampAide PRIVATE WAMPAIDE_VERSION="${PROJECT_VERSION}")

if(MSVC)
    target_compile_options(wampAide PRIVATE /W4)
else()
    target_compile_options(wampAide PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Statically link runtime libraries so wampAide.exe has no external runtime DLL dependencies
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(wampAide PRIVATE -static -static-libgcc -static-libstdc++)
endif()
