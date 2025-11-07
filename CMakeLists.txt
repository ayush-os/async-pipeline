cmake_minimum_required(VERSION 3.10)
project(AsynchronousPipeline CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_TSAN "Enable Thread Sanitizer for race condition detection" OFF)

add_executable(pipeline main.cpp include/safe_queue.h)

target_include_directories(pipeline PUBLIC "${PROJECT_SOURCE_DIR}/include")

if (ENABLE_TSAN)
    message(STATUS "Compiling with Thread Sanitizer (TSAN) enabled.")

    target_compile_options(pipeline PUBLIC -fsanitize=thread -g)
    target_link_options(pipeline PUBLIC -fsanitize=thread)

    target_compile_options(pipeline PUBLIC -O0 -fno-omit-frame-pointer)
else()
    message(STATUS "Compiling with standard optimization.")
    target_compile_options(pipeline PUBLIC -O3)
endif()