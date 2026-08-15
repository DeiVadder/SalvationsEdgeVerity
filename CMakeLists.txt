cmake_minimum_required(VERSION 3.21)

project(SalvationsEdgeVerity VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # No -Wpedantic: Qt's own generated code (e.g. qmlcache_loader.cpp's
    # Q_GLOBAL_STATIC expansion) trips -Wvariadic-macro-arguments-omitted
    # under C++17, which isn't something we can or should fix.
    add_compile_options(-Wall -Wextra)
endif()

find_package(Qt6 6.11 REQUIRED COMPONENTS Quick Svg Test LinguistTools)

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
    cpp/i18n/translationmanager.cpp
    cpp/i18n/translationmanager.h
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
        qml/help/HelpDialog.qml
)

target_sources(SalvationsEdgeVerity PRIVATE rsc.qrc)

target_link_libraries(SalvationsEdgeVerity PRIVATE
    Qt6::Quick
    Qt6::Svg
)

# English (qsTr() source strings) + German. TS_FILES listed explicitly so
# the embedded resource path is predictable (:/i18n/salvationsedgeverity_de.qm)
# for TranslationManager to load by name - see cpp/i18n/translationmanager.cpp.
qt_add_translations(SalvationsEdgeVerity
    TS_FILES translations/salvationsedgeverity_de.ts
)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
