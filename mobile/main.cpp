#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "services/scanservice.h"
#include "services/deviceservice.h"
#include "services/usersettingsservice.h"
#include <QDebug>

#if defined (Q_OS_ANDROID)
#include <QtAndroid>
const QVector<QString> permissions({"android.permission.ACCESS_COARSE_LOCATION"});
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    #if defined(Q_OS_ANDROID)
    // request permissions at runtime if not already approved
    for (const QString &permission : permissions)
    {
        auto result = QtAndroid::checkPermission(permission);
        if (result == QtAndroid::PermissionResult::Denied)
        {
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if ( resultHash[permission] == QtAndroid::PermissionResult::Denied)
            {
                qCritical() << "Insufficient permissions, application terminated";
                return 0;
            }
        }
    }
    #endif

    UserSettingsService userSettings;
    DeviceService deviceService;
    ScanService scanService(&deviceService, &userSettings);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("userSettings", &userSettings);
    engine.rootContext()->setContextProperty("scanService", &scanService);
    engine.rootContext()->setContextProperty("deviceService", &deviceService);

    // register property to mimic preprocessor decision making in QML
    #if defined(Q_OS_ANDROID)
    engine.rootContext()->setContextProperty("QML_OS_ANDROID", QVariant(true));
    #elif
    engine.rootContext()->setContextProperty("QML_OS_ANDROID", QVariant(false));
    #endif

    const QUrl url = QStringLiteral("qrc:/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
