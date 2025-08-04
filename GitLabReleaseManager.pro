QT = core gui sql network quick


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# CONFIG += qmltypes
# QML_IMPORT_NAME = qmlcomponents
# QML_IMPORT_MAJOR_VERSION = 1

SOURCES += \
    main.cpp \
    src/databaseworker.cpp \
    src/gitworker.cpp \
    src/privatekeymodel.cpp \
    src/requester.cpp

HEADERS += \
    src/databaseworker.h \
    src/gitworker.h \
    src/global.h \
    src/privatekeymodel.h \
    src/requester.h

RESOURCES += \
    resources.qrc \
    style.qrc

CONFIG += qmltypes
QML_IMPORT_NAME = qmlcomponents
QML_IMPORT_MAJOR_VERSION = 1
QML_IMPORT_PATH += $$PWD/

INCLUDEPATH += src
