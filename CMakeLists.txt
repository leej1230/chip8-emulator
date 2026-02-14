cmake_minimum_required(VERSION 4.2.1)

project(chip8-emulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "amd64|AMD64|x86_64")
        add_compile_definitions(_AMD64_)
    endif()
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86|i386")
        add_compile_definitions(_X86_)
    endif()
endif()

include(FetchContent)

set(DSDL_SHARED OFF CACHE BOOL "Build Shared Lib" FORCE)
set(DSDL_STATIC ON CACHE BOOL "Build Static Lib" FORCE)
set(DSDL_TESTS OFF CACHE BOOL "Build Test" FORCE)
FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.0
)
FetchContent_MakeAvailable(SDL3)

add_executable(chip8-emulator main.cpp)

target_include_directories(chip8-emulator PRIVATE
    ${SDL3_SOURCE_DIR}/include
)

target_link_libraries(chip8-emulator PRIVATE
    SDL3::SDL3
)

# windows does not have rpath 
if(WIN32)
    add_custom_command(TARGET chip8-emulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:chip8-emulator>
    )
endif()
