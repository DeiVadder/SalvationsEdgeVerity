#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "puzzle/calculateinsidesteps.h"
#include "puzzle/calculatesteps.h"
#include "challenge/encounterprogress.h"
#include "ghost/ghostphasehelper.h"
#include "i18n/translationmanager.h"

int main(int argc, char *argv[])
{
    qputenv("QSG_RHI", "1");
    QCoreApplication::setOrganizationName(QStringLiteral("DeiVadder"));
    QCoreApplication::setApplicationName(QStringLiteral("SalvationsEdgeVerity"));
    QGuiApplication app(argc, argv);

    qmlRegisterType<CalculateSteps>("CalculateSteps", 1, 0, "CalculateSteps");
    qmlRegisterUncreatableType<CalculateSteps>("SymbolEnums", 1, 0, "Symbols", "Enum for Symbols");
    qmlRegisterType<CalculateInsideSteps>("CalculateSteps", 1, 0, "CalculateInsideSteps");
    qmlRegisterType<EncounterProgress>("CalculateSteps", 1, 0, "EncounterProgress");
    qmlRegisterType<GhostPhaseHelper>("CalculateSteps", 1, 0, "GhostPhaseHelper");

    // Installs the saved/system-default translator before any QML is
    // loaded, so the very first frame already renders in the right
    // language without needing a retranslate() pass.
    TranslationManager translationManager;
    translationManager.initialize();

    QQmlApplicationEngine engine;
    translationManager.setEngine(&engine);
    engine.rootContext()->setContextProperty("translationManager", &translationManager);
    const QUrl url(QStringLiteral("qrc:/qt/qml/SalvationsEdgeVerityApp/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
