#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "calculatesteps.h"

int main(int argc, char *argv[])
{
    qputenv("QSG_RHI", "1");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qmlRegisterType<CalculateSteps>("CalculateSteps", 1, 0, "CalculateSteps");
    qmlRegisterUncreatableType<CalculateSteps>("SymbolEnums", 1, 0, "Symbols", "Enum for Symbols");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
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
