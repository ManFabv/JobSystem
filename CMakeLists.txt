# minimum CMake version: 3.16 is old enough to be everywhere and supports everything used here
cmake_minimum_required(VERSION 3.16)

# project name and language (CXX = C++); this also detects the compiler
project(JobSystem LANGUAGES CXX)

# ------------------------------------------------------------
# Language standard
# ------------------------------------------------------------
# we compile as C++11
set(CMAKE_CXX_STANDARD 11)
# fail if the compiler does not support C++11 (instead of silently using an older standard)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# -std=c++11 instead of -std=gnu++11 (no compiler specific extensions)
set(CMAKE_CXX_EXTENSIONS OFF)
# generate compile_commands.json (used by clangd, CLion, VS Code for autocompletion)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Options (cmake -DJOBSYSTEM_ENABLE_TSAN=ON ...)
# ------------------------------------------------------------
option(JOBSYSTEM_ENABLE_TSAN "Build with ThreadSanitizer (clang/gcc only)" OFF)

# ------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------
# portable way of linking the platform thread library
# (on macOS it adds nothing, on Linux it adds -pthread)
find_package(Threads REQUIRED)

# ------------------------------------------------------------
# Targets
# ------------------------------------------------------------
# the job system as a static library, so later the ECS (or tests) can link it
add_library(JobSystemLib STATIC
    JobSystem.cpp
    JobSystem.hpp
)
# whoever links JobSystemLib can #include "JobSystem.hpp" (PUBLIC = propagates to dependents)
target_include_directories(JobSystemLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# the header includes <thread>, so dependents also need the thread library (PUBLIC)
target_link_libraries(JobSystemLib PUBLIC Threads::Threads)

# the demo executable (the main.cpp of the current lesson)
add_executable(JobSystemDemo main.cpp)
# PRIVATE: nobody links against an executable, so nothing to propagate
target_link_libraries(JobSystemDemo PRIVATE JobSystemLib)

# ------------------------------------------------------------
# Warnings and sanitizers for both targets
# ------------------------------------------------------------
foreach(target_name JobSystemLib JobSystemDemo)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra)
        if(JOBSYSTEM_ENABLE_TSAN)
            target_compile_options(${target_name} PRIVATE -fsanitize=thread -g)
            target_link_options(${target_name} PRIVATE -fsanitize=thread)
        endif()
    endif()
endforeach()
