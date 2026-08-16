qt_add_executable(tst_calculatesteps
    tst_calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.h
)
target_include_directories(tst_calculatesteps PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_calculatesteps PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_calculatesteps COMMAND tst_calculatesteps)

qt_add_executable(tst_symbolswapengine
    tst_symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.h
)
target_include_directories(tst_symbolswapengine PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_symbolswapengine PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_symbolswapengine COMMAND tst_symbolswapengine)

qt_add_executable(tst_encounterprogress
    tst_encounterprogress.cpp
    ${CMAKE_SOURCE_DIR}/cpp/challenge/encounterprogress.cpp
    ${CMAKE_SOURCE_DIR}/cpp/challenge/encounterprogress.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.h
)
target_include_directories(tst_encounterprogress PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_encounterprogress PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_encounterprogress COMMAND tst_encounterprogress)

qt_add_executable(tst_calculateinsidesteps
    tst_calculateinsidesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculateinsidesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculateinsidesteps.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/fastcleanseresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/fastcleanseresolver.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgsortresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgsortresolver.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgdistributeresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgdistributeresolver.h
)
target_include_directories(tst_calculateinsidesteps PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_calculateinsidesteps PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_calculateinsidesteps COMMAND tst_calculateinsidesteps)

qt_add_executable(tst_fastcleanseresolver
    tst_fastcleanseresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/fastcleanseresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/fastcleanseresolver.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.h
)
target_include_directories(tst_fastcleanseresolver PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_fastcleanseresolver PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_fastcleanseresolver COMMAND tst_fastcleanseresolver)

qt_add_executable(tst_lfgsortresolver
    tst_lfgsortresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgsortresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgsortresolver.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.h
)
target_include_directories(tst_lfgsortresolver PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_lfgsortresolver PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_lfgsortresolver COMMAND tst_lfgsortresolver)

qt_add_executable(tst_lfgdistributeresolver
    tst_lfgdistributeresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgdistributeresolver.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/lfgdistributeresolver.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/calculatesteps.h
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.cpp
    ${CMAKE_SOURCE_DIR}/cpp/puzzle/symbolswapengine.h
)
target_include_directories(tst_lfgdistributeresolver PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_lfgdistributeresolver PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_lfgdistributeresolver COMMAND tst_lfgdistributeresolver)

qt_add_executable(tst_ghostplayerslot
    tst_ghostplayerslot.cpp
    ${CMAKE_SOURCE_DIR}/cpp/ghost/ghostplayerslot.cpp
    ${CMAKE_SOURCE_DIR}/cpp/ghost/ghostplayerslot.h
)
target_include_directories(tst_ghostplayerslot PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_ghostplayerslot PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_ghostplayerslot COMMAND tst_ghostplayerslot)

qt_add_executable(tst_ghostphasehelper
    tst_ghostphasehelper.cpp
    ${CMAKE_SOURCE_DIR}/cpp/ghost/ghostphasehelper.cpp
    ${CMAKE_SOURCE_DIR}/cpp/ghost/ghostphasehelper.h
    ${CMAKE_SOURCE_DIR}/cpp/ghost/ghostplayerslot.cpp
    ${CMAKE_SOURCE_DIR}/cpp/ghost/ghostplayerslot.h
)
target_include_directories(tst_ghostphasehelper PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
target_link_libraries(tst_ghostphasehelper PRIVATE Qt6::Test Qt6::Core)
add_test(NAME tst_ghostphasehelper COMMAND tst_ghostphasehelper)
