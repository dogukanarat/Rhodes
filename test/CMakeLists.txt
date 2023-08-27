message("[rhodeus] Test enabled")

enable_testing()

find_package(GTest REQUIRED)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

add_executable(${PROJECT_NAME}Test)

set(
    TEST_SRC
    ${SRC}
    ${CMAKE_CURRENT_LIST_DIR}/Main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Message.cpp
)
set(
    TEST_INC
    ${INC}
    ${CMAKE_CURRENT_LIST_DIR}
)
target_compile_definitions(${PROJECT_NAME}Test PUBLIC CTEST)
target_sources(${PROJECT_NAME}Test PRIVATE ${TEST_SRC})
target_include_directories(${PROJECT_NAME}Test PRIVATE ${TEST_INC})
target_link_libraries(${PROJECT_NAME}Test PRIVATE ${LIBS} GTest::gtest_main)

gtest_discover_tests(${PROJECT_NAME}Test
    PROPERTIES
      LABELS "unit"
    DISCOVERY_TIMEOUT
      240
    )
