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
    src/privatekeymodel.cpp \
    src/projects/project.cpp \
    src/projects/projectslistmodel.cpp \
    src/projects/releaselistmodel.cpp \
    src/requester.cpp

HEADERS += \
    src/databaseworker.h \
    src/global.h \
    src/privatekeymodel.h \
    src/projects/project.h \
    src/projects/projectslistmodel.h \
    src/projects/releaselistmodel.h \
    src/requester.h

RESOURCES += \
    resources.qrc \
    style.qrc

CONFIG += qmltypes
QML_IMPORT_NAME = qmlcomponents
QML_IMPORT_MAJOR_VERSION = 1
QML_IMPORT_PATH += $$PWD/

INCLUDEPATH += $$PWD/src $$PWD/src/projects

TRANSLATIONS += \
    GitLabReleaseManager_ru_RU.ts
