#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "scanservice.h"
#include "deviceservice.h"
#include "usersettingsservice.h"

#if defined (Q_OS_ANDROID)
#include <QtAndroid>
const QVector<QString> permissions();
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    #if defined(Q_OS_ANDROID)
    // request permissions at runtime if not already approved
    if (true);
    // TODO determine if I only need the android manifest file in the android folder or the gradel config as well?
    // https://amin-ahmadi.com/2015/11/29/how-to-add-permissions-to-your-qt-android-app/

    // TODO Also how to include Qt packages in apk instead of shared through Ministro
    // https://doc.qt.io/qtcreator/creator-deploying-android.html

    // TODO

    // Starting in API level 19, this permission is not required to read/write files in your application-specific directories returned by Context.getExternalFilesDir(String) and Context.getExternalCacheDir().
    #endif

    UserSettingsService userSettings;
    DeviceService deviceService;
    ScanService scanService(&deviceService, &userSettings);

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
