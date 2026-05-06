cmake_minimum_required(VERSION 3.16)
project(new_proj_3 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
include_directories(${SFML_INCLUDE_DIRS})


add_executable(new_proj_3 main.cpp
        tile.h
)
target_link_libraries(new_proj_3 PRIVATE sfml-graphics sfml-window sfml-system)
target_compile_features(new_proj_3 PRIVATE cxx_std_17)

set(CMAKE_VERBOSE_MAKEFILE ON)


if(WIN32)
    add_custom_command(
            TARGET new_proj_3
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:new_proj_3>
            VERBATIM)
endif()