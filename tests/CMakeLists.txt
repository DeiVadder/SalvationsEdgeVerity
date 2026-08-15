qt_add_executable(tst_calculatesteps
    tst_calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculatesteps.h
)
target_include_directories(tst_calculatesteps PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tst_calculatesteps PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_calculatesteps COMMAND tst_calculatesteps)
