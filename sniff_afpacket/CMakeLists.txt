cmake_minimum_required(VERSION 3.10) # Minimum version that supports C++20 features
project(SNIFF_PACKET LANGUAGES C)

# Set default compiler to C 17
set(CMAKE_C_COMPILER "gcc" CACHE STRING "C compiler (gcc/clang)")
# set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler (g++/clang++)")
set(CMAKE_VERBOSE_MAKEFILE ON)

# Verify compiler
if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "C compiler not found!")
else()
    message(STATUS "Using C compiler: ${CMAKE_C_COMPILER}")
    execute_process(COMMAND ${CMAKE_C_COMPILER} --version OUTPUT_VARIABLE COMPILER_VERSION)
    message(STATUS "Compiler version:\n${COMPILER_VERSION}")
endif()

# Compiler specific configuration
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    message(STATUS "Configuring for GCC")
    # GCC-specific flags
    add_compile_options(
        -fgnu89-inline
        -flto
        -fstack-protector-strong
        --param=ssp-buffer-size=4
    )

    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")

elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "Configuring for Clang")
    # Clang-specific flags
    add_compile_options(
        -fgnu89-inline
        -flto=thin
        -fsanitize=safe-stack
    )

    # Different flag for equivalent functionality
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

# Common GNU extensions configuration
set(CMAKE_C_EXTENSIONS ON)
add_compile_definitions(GNU_SOURCE)
add_compile_options(
  -Wextra
  -Wpedantic
  -Wno-overlength-strings
)

# Check if DEBUG is set on command line
if(DEBUG)
    add_definitions(-DDEBUG)
endif()

# Enable testing
enable_testing()

# Add directories to include source files
include_directories(${PROJECT_SOURCE_DIR}/src)

# Include directories for external libraries (e.g., librdkafka, libmemcached)
find_package(PkgConfig REQUIRED) # pkgconfig folder: /usr/local/lib/pkgconfig

# Find librdkafka
pkg_check_modules(LIBRDKAFKA REQUIRED rdkafka)
include_directories(${LIBRDKAFKA_INCLUDE_DIRS})
link_directories(${LIBRDKAFKA_LIBRARY_DIRS})

# Find memcached
# If install using apt install libmemcached-dev
# /usr/lib/x86_64-linux-gnu/libmemcached.a
# /usr/lib/x86_64-linux-gnu/libmemcached.so
# /usr/include/libmemcached
find_library(MEMCACHED_LIBRARY memcached REQUIRED)
include_directories(/usr/include)


#########
# BUILD #
#########
# Add source files from subdirectories within 'src'
file(GLOB_RECURSE SRC_FILES "src/*.c" "src/**/*.c" "src/**/*.h")
list(FILTER SRC_FILES EXCLUDE REGEX "test_*\\.c$")  # Exclude test files

# Create an executable for your main project
add_executable(sniff_packet ${SRC_FILES})

target_link_libraries(sniff_packet ${LIBRDKAFKA_LIBRARIES} ${MEMCACHED_LIBRARY} pthread)
