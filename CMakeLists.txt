cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0079 NEW)

# Set version
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/version.sh VERSION ${INCREMENT_VERSION}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE CUSTOM_VERSION)
else()
    set(CMAKE_BUILD_TYPE Debug)
    set(CUSTOM_VERSION 0.0.0)
endif()

# Set the project name
project(Rhodeus VERSION ${CUSTOM_VERSION} DESCRIPTION "Simple CPP god project")

message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "CUSTOM_VERSION = ${CUSTOM_VERSION}")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(RHODEUS_TEST_ENABLED "Enable testing" ON)

# Define compiler flags
add_compile_options(-pedantic -Wall -Wextra)

# GNU standard installation directories
include(GNUInstallDirs)

message("Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message("Compiler: ${CMAKE_CXX_COMPILER}")
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message("System: ${CMAKE_SYSTEM_NAME}")
message("System Version: ${CMAKE_SYSTEM_VERSION}")
message("System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("Build Type: ${CMAKE_BUILD_TYPE}")
message("C Version: ${CMAKE_C_STANDARD}")
message("C++ Version: ${CMAKE_CXX_STANDARD}")

configure_file(${PROJECT_SOURCE_DIR}/include/rhodeus/RhodeusAutoConfig.hpp.in
               ${PROJECT_BINARY_DIR}/rhodeus/RhodeusAutoConfig.hpp)

find_package(PkgConfig REQUIRED)
find_package(fmt REQUIRED)
find_package(plog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)
find_package(linenoisecli REQUIRED)

set(DEP_INCLUDE_DIRS
    ${linenoisecli_INCLUDE_DIRS}
    )

set(DEP_LIBRARIES
    fmt::fmt
    plog::plog
    nlohmann_json::nlohmann_json
    cppzmq
    ${linenoisecli_LIBRARIES}
    )

set(SOURCES
    src/Application.cpp
    src/Component.cpp
    src/Setup.cpp
    src/Configuration/Configuration.cpp
    src/Ipc/Ipc.cpp
    src/Ipc/IpcClient.cpp
    src/Ipc/IpcServer.cpp
    src/Message.cpp
    src/Helper.cpp
    )

set(PUB_HEADERS
    )

set(PROJECT_APP_NAME ${PROJECT_NAME}App)
set(PROJECT_LIB_NAME ${PROJECT_NAME}Lib)

add_library(${PROJECT_LIB_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_LIB_NAME} PUBLIC include)
target_include_directories(${PROJECT_LIB_NAME} PRIVATE src)
target_include_directories(${PROJECT_LIB_NAME} PRIVATE ${DEP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_LIB_NAME} PRIVATE ${DEP_LIBRARIES})

add_executable(${PROJECT_APP_NAME} src/Main.cpp)
target_include_directories(${PROJECT_APP_NAME} PUBLIC include)
target_include_directories(${PROJECT_APP_NAME} PRIVATE src)
target_include_directories(${PROJECT_APP_NAME} PRIVATE ${DEP_INCLUDE_DIRS})

target_link_libraries(${PROJECT_APP_NAME} PRIVATE ${DEP_LIBRARIES} ${PROJECT_LIB_NAME})

if (RHODEUS_TEST_ENABLED STREQUAL "ON")
    add_subdirectory(test)
endif()
