cmake_minimum_required(VERSION 4.0)

project(fizzlang 
        VERSION 0.0.1
        DESCRIPTION "Custom programming language"
        LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMMANDS ON)

set(CMAKE_C_STANDARD 99)
set(ASAN_CFLAGS -fsanitize=address,undefined -g -Wall -Wextra -fno-omit-frame-pointer)

include(FetchContent)

FetchContent_Declare(
  cstring.h
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/cstring.h
  GIT_REPOSITORY https://github.com/mirsahebali/cstring.h
  GIT_TAG main
)

FetchContent_MakeAvailable(cstring.h)

set(CORE_FILES arena.c lexer.c arrays.c ast.c parser.c repl.c utils.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps)

add_library(${PROJECT_NAME}-coredebug STATIC ${CORE_FILES})
target_compile_options(${PROJECT_NAME}-coredebug PUBLIC ${ASAN_CFLAGS})
target_link_options(${PROJECT_NAME}-coredebug PUBLIC )

add_executable(${PROJECT_NAME}-debug main.c)
target_compile_options(${PROJECT_NAME}-debug PUBLIC ${ASAN_CFLAGS})

add_executable(${PROJECT_NAME}-tests tests.c)
target_compile_options(${PROJECT_NAME}-tests PUBLIC ${ASAN_CFLAGS})

target_link_libraries(${PROJECT_NAME}-debug PUBLIC ${PROJECT_NAME}-coredebug)
target_link_options(${PROJECT_NAME}-debug PUBLIC ${ASAN_CFLAGS})
target_link_libraries(${PROJECT_NAME}-tests PUBLIC ${PROJECT_NAME}-coredebug)
target_link_options(${PROJECT_NAME}-tests PUBLIC ${ASAN_CFLAGS})

