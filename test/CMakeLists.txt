message("[rhodeus] Test enabled")

enable_testing()

find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME}Test)

set(TEST_SRC
    ${CMAKE_CURRENT_LIST_DIR}/TestVector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TestMessage.cpp
)

set(TEST_INC ${INC} ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(${PROJECT_NAME}Test PUBLIC CTEST)
target_sources(${PROJECT_NAME}Test PRIVATE ${TEST_SRC})
target_include_directories(${PROJECT_NAME}Test PRIVATE ${TEST_INC})
target_link_libraries(${PROJECT_NAME}Test PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_NAME}Test PRIVATE RhodeusLib ${LIBS})

add_test(NAME ${PROJECT_NAME}Test COMMAND ${PROJECT_NAME}Test)
