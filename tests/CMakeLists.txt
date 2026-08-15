qt_add_executable(tst_calculatesteps
    tst_calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/symbolswapengine.h
)
target_include_directories(tst_calculatesteps PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tst_calculatesteps PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_calculatesteps COMMAND tst_calculatesteps)

qt_add_executable(tst_symbolswapengine
    tst_symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/symbolswapengine.h
    ${CMAKE_SOURCE_DIR}/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculatesteps.h
)
target_include_directories(tst_symbolswapengine PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tst_symbolswapengine PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_symbolswapengine COMMAND tst_symbolswapengine)

qt_add_executable(tst_encounterprogress
    tst_encounterprogress.cpp
    ${CMAKE_SOURCE_DIR}/encounterprogress.cpp
    ${CMAKE_SOURCE_DIR}/encounterprogress.h
    ${CMAKE_SOURCE_DIR}/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/symbolswapengine.h
)
target_include_directories(tst_encounterprogress PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tst_encounterprogress PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_encounterprogress COMMAND tst_encounterprogress)

qt_add_executable(tst_calculateinsidesteps
    tst_calculateinsidesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculateinsidesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculateinsidesteps.h
    ${CMAKE_SOURCE_DIR}/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/symbolswapengine.h
)
target_include_directories(tst_calculateinsidesteps PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tst_calculateinsidesteps PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_calculateinsidesteps COMMAND tst_calculateinsidesteps)
