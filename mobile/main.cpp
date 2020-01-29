#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "scanservice.h"
#include "usersettingsservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    UserSettingsService userSettings;
    DeviceService deviceService;
    ScanService scanService(&deviceService);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("userSettings", &userSettings);
    engine.rootContext()->setContextProperty("scanService", &scanService);
    engine.rootContext()->setContextProperty("deviceService", &deviceService);

    const QUrl url = QStringLiteral("qrc:/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
