cmake_minimum_required(VERSION 3.21)

project(SalvationsEdgeVerity VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.11 REQUIRED COMPONENTS Quick Svg Test)

qt_standard_project_setup(REQUIRES 6.11)

qt_add_executable(SalvationsEdgeVerity
    main.cpp
    cpp/puzzle/calculatesteps.cpp
    cpp/puzzle/calculatesteps.h
    cpp/puzzle/symbolswapengine.cpp
    cpp/puzzle/symbolswapengine.h
    cpp/puzzle/calculateinsidesteps.cpp
    cpp/puzzle/calculateinsidesteps.h
    cpp/puzzle/fastcleanseresolver.cpp
    cpp/puzzle/fastcleanseresolver.h
    cpp/challenge/encounterprogress.cpp
    cpp/challenge/encounterprogress.h
    cpp/ghost/ghostplayerslot.cpp
    cpp/ghost/ghostplayerslot.h
    cpp/ghost/ghostphasehelper.cpp
    cpp/ghost/ghostphasehelper.h
)

target_include_directories(SalvationsEdgeVerity PRIVATE ${CMAKE_SOURCE_DIR}/cpp)

qt_add_qml_module(SalvationsEdgeVerity
    URI SalvationsEdgeVerityApp
    VERSION 1.0
    QML_FILES
        main.qml
        qml/js/ShapeIcons.js
        qml/js/ShapeMath.js
        qml/panels/ShapeGridSelector.qml
        qml/panels/StepCard.qml
        qml/panels/InputPanel.qml
        qml/panels/SolutionPanel.qml
        qml/panels/InsideRoomPanel.qml
        qml/challenge/ShapeUsageTracker.qml
        qml/ghost/GhostPlayerSlotCard.qml
        qml/ghost/GhostPhaseHelperPanel.qml
)

target_sources(SalvationsEdgeVerity PRIVATE rsc.qrc)

target_link_libraries(SalvationsEdgeVerity PRIVATE
    Qt6::Quick
    Qt6::Svg
)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
