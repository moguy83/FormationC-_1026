set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3 REQUIRED)

add_executable(tp8C tp8C.cpp)

target_link_libraries(tp8C PRIVATE Catch2::Catch2WithMain)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(tp8C PRIVATE -O0 -g --coverage)

    target_link_options(tp8C PRIVATE --coverage)

endif()

enable_testing()
add_test(NAME Couverture COMMAND tp8C)

//cmake -S . -B build
// cmake --build build 
// ctest --test-dir build --output-on-failure

// gcovr --root . --object-directory build --filter '.*remise\.hpp' --txt 
// gcovr --root . --object-directory build --filter '.*remise\.hpp' --branches --txt 
