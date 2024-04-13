find_package(PkgConfig)
pkg_check_modules(LinenoiseCLI linenoisecli)

find_path(Find_LinenoiseCLI_INCLUDE_DIRS linenoisecli.hpp
          PATHS ${LinenoiseCLI_INCLUDE_DIRS_HINT}
                ${LinenoiseCLI_INCLUDE_DIRS}
                /usr/include
                /usr/local/include
                /usr/include/linenoisecli
                /usr/local/include/linenoisecli)

find_library(Find_LinenoiseCLI_LIBRARIES NAMES linenoisecli
                PATHS ${LinenoiseCLI_LIBRARIES_HINT}
                    ${LinenoiseCLI_LIBRARY_DIRS}
                    /usr/lib
                    /usr/local/lib
                    /usr/lib/linenoise
                    /usr/local/lib/linenoise)

if(Find_LinenoiseCLI_LIBRARIES AND Find_LinenoiseCLI_INCLUDE_DIRS)
    set(LinenoiseCLI_FOUND ON)
    message(STATUS "Found LinenoiseCLI")
else()
    set(LinenoiseCLI_FOUND OFF)
    message(STATUS "LinenoiseCLI not found")
endif()

message(STATUS "LinenoiseCLI libraries: ${Find_LinenoiseCLI_LIBRARIES}")
message(STATUS "LinenoiseCLI include directories: ${Find_LinenoiseCLI_INCLUDE_DIRS}")

set(linenoisecli_LIBRARIES ${Find_LinenoiseCLI_LIBRARIES})
set(linenoisecli_INCLUDE_DIRS ${Find_LinenoiseCLI_INCLUDE_DIRS})
