enable_testing()

find_package(Catch2 REQUIRED)

set(PROJECT_TEST_NAME ${PROJECT_NAME}Test)

set(TEST_SOURCE
    TestVector.cpp
    TestMessage.cpp
    )

add_executable(${PROJECT_TEST_NAME} ${TEST_SOURCE})

target_link_libraries(${PROJECT_TEST_NAME} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_TEST_NAME} PRIVATE ${PROJECT_LIB_NAME})

add_test(NAME ${PROJECT_TEST_NAME} COMMAND ${PROJECT_TEST_NAME})
