cmake_minimum_required(VERSION 3.6)

project(
    Rhodeus
    VERSION 0.1
    LANGUAGES CXX
    )

include(Options.cmake)

# set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(PkgConfig REQUIRED)
find_package(fmt REQUIRED)
find_package(plog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost COMPONENTS filesystem iostreams system REQUIRED)
find_package(cppzmq REQUIRED)
find_package(ftxui REQUIRED)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

message("[rhodeus] Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message("[rhodeus] Compiler: ${CMAKE_CXX_COMPILER}")
message("[rhodeus] Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message("[rhodeus] System: ${CMAKE_SYSTEM_NAME}")
message("[rhodeus] System Version: ${CMAKE_SYSTEM_VERSION}")
message("[rhodeus] System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("[rhodeus] Build Type: ${CMAKE_BUILD_TYPE}")
message("[rhodeus] C Version: ${CMAKE_C_STANDARD}")
message("[rhodeus] C++ Version: ${CMAKE_CXX_STANDARD}")

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Setup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Configuration/Configuration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Ipc/Ipc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Ipc/IpcClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Ipc/IpcServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Message.cpp
    )

set(INC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${Boost_INCLUDE_DIRS}
    )

set(LIBS
    fmt::fmt
    plog::plog
    nlohmann_json::nlohmann_json
    cppzmq
    ftxui::screen
    ftxui::dom
    ftxui::component
    ${Boost_LIBRARIES}
    )

add_executable(Rhodeus
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${SRC})

target_include_directories(Rhodeus
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_include_directories(Rhodeus
    PUBLIC
    ${INC}
    )

target_link_libraries(Rhodeus
    PRIVATE
    ${LIBS}
    )

if (RHODEUS_TEST_ENABLED)
    add_subdirectory(test)
endif()

