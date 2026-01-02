cmake_minimum_required(VERSION 3.16)

# Boost policy warning suppression (newer CMake)
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(weather-alerts LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.74 REQUIRED COMPONENTS program_options)

add_executable(weather-alerts
  weather-alerts.cpp
)

target_link_libraries(weather-alerts PRIVATE
  CURL::libcurl
  Threads::Threads
  Boost::program_options
)

set(GIT_VERSION "unknown")
find_package(Git QUIET)
if(GIT_FOUND)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --tags --dirty --always
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
endif()

message(STATUS "GIT_VERSION: ${GIT_VERSION}")

target_compile_definitions(weather-alerts PRIVATE
  GIT_VERSION="${GIT_VERSION}"
)

