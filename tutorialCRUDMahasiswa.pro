QT       += core gui sql printsupport
QT       -= network svg xml dbus virtualkeyboard quick qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 release

QMAKE_LFLAGS_RELEASE += -dead_strip

RC_ICONS = $$PWD/logos/logo_crudMahasiswa_512px.ico
ICON = $$PWD/logos/logo_crudMahasiswa_512px.icns

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

VERSION += 1.0
# APPNAME += "CRUD Mahasiswa"

DEFINES += APP_NAME=\\\"CRUDMahasiswa\\\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include(modules/CSVExporter/CSVExporter.pri)
include(modules/PDFExporter/PDFExporter.pri)

SOURCES += \
    dialogs/AboutDialog/aboutdialog.cpp \
    helpers/databasemanager.cpp \
    main.cpp \
    mainwindow.cpp \
    models/tablemodel.cpp

HEADERS += \
    helpers/Environments.h \
    dialogs/AboutDialog/aboutdialog.h \
    helpers/databasemanager.h \
    mainwindow.h \
    models/tablemodel.h

FORMS += \
    dialogs/AboutDialog/aboutdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    darkcss/darkstyle.qss

RESOURCES += \
    Res.qrc \
    darkcss/darkstyle.qrc
