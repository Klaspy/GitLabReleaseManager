#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QTranslator>

#include "src/databaseworker.h"
#include "src/privatekeymodel.h"
#include "src/projects/projectslistmodel.h"

#include <QDirIterator>

int main (int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine(&app);
    engine.addImportPath("qrc:/");

    QTranslator translator;
    if (translator.load(QLocale(), "GitLabReleaseManager", "_", ":/translations"))
    {
        QGuiApplication::installTranslator(&translator);
        engine.retranslate();
    }

    PrivateKeyModel *pKeyModel = new PrivateKeyModel(&app);
    engine.rootContext()->setContextProperty("PKeyModel", pKeyModel);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []()
                     {
                         QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    // engine.load(QUrl("qrc:/qml/App.qml"));
    engine.loadFromModule("GitLabReleaseManagerModule", "App");

    return app.exec();
}
