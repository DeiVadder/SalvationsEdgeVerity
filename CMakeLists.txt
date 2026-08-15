cmake_minimum_required(VERSION 3.21)

project(SalvationsEdgeVerity VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.11 REQUIRED COMPONENTS Quick Svg Test)

qt_standard_project_setup(REQUIRES 6.11)

qt_add_executable(SalvationsEdgeVerity
    main.cpp
    calculatesteps.cpp
    calculatesteps.h
    symbolswapengine.cpp
    symbolswapengine.h
    encounterprogress.cpp
    encounterprogress.h
    calculateinsidesteps.cpp
    calculateinsidesteps.h
)

qt_add_qml_module(SalvationsEdgeVerity
    URI SalvationsEdgeVerityApp
    VERSION 1.0
    QML_FILES
        main.qml
        qml/js/ShapeIcons.js
        qml/panels/ShapeGridSelector.qml
        qml/panels/StepCard.qml
        qml/panels/InputPanel.qml
        qml/panels/SolutionPanel.qml
        qml/panels/InsideRoomPanel.qml
        qml/challenge/ShapeUsageTracker.qml
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
