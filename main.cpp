#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/databaseworker.h"
#include "src/privatekeymodel.h"

int main (int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine(&app);
    engine.addImportPath("qrc:/");

    DatabaseWorker *dbWorker = new DatabaseWorker(&app);

    PrivateKeyModel *pKeyModel = new PrivateKeyModel(dbWorker, &app);
    engine.rootContext()->setContextProperty("PKeyModel", pKeyModel);

    qmlRegisterSingletonInstance("qmlcomponents", 1, 0, "DatabaseWorker", dbWorker);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []()
                     {
                         QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(QUrl("qrc:/qml/App.qml"));

    return app.exec();
}
