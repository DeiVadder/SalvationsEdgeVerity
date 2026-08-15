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
)

qt_add_qml_module(SalvationsEdgeVerity
    URI SalvationsEdgeVerityApp
    VERSION 1.0
    QML_FILES
        main.qml
        OutsideRoom.qml
        StatueStateMachine.qml
        IconButton.qml
        StatemachineRow.qml
        qml/Layouts/LayoutInRow.qml
        qml/Layouts/VisualStateMachineForRow.qml
        qml/shapes/Selection2d.qml
        qml/shapes/Selection3dShape.qml
        qml/shapes/ShapePopup2d.qml
        qml/shapes/ShapePopup3d.qml
        qml/steps/StepX.qml
        qml/Layouts/LayoutInColumn.qml
        qml/Layouts/VisualStateMachineForCol.qml
        qml/Layouts/LayoutInRowCol.qml
        qml/Layouts/InputSymbols.qml
        qml/Layouts/VisualSteps.qml
)

target_sources(SalvationsEdgeVerity PRIVATE rsc.qrc)

target_link_libraries(SalvationsEdgeVerity PRIVATE
    Qt6::Quick
    Qt6::Svg
)

# if(ANDROID)
#     set_target_properties(SalvationsEdgeVerity PROPERTIES
#         QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
#     )
# endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
