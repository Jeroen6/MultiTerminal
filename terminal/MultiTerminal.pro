#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T19:55:09
#
#-------------------------------------------------
VERSION = 1.0.0.0
QMAKE_TARGET_PRODUCT = "MultiTerminal"
QMAKE_TARGET_COMPANY = "github.com/jeroen6"
QMAKE_TARGET_DESCRIPTION = "MultiTerminal TCP Serial Terminal"
QMAKE_TARGET_COPYRIGHT = "Copyright (c) github.com/jeroen6"

DEFINES += APP_VERSION=\"\\\"$${VERSION}\\\"\" \
           APP_PRODUCT=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\" \
           APP_COMPANY=\"\\\"$${QMAKE_TARGET_COMPANY}\\\"\" \
           APP_COPYRIGHT=\"\\\"$${QMAKE_TARGET_COPYRIGHT}\\\"\"

RC_ICONS = resources/icons/1394398324_103542.ico

# Note slow build: Projects -> Build steps -> Make
# Add make argument: -j
# For multithreaded build. Hint: -j4 for 4 cores
MAKEFLAGS = -j

# file_copies is an undocumented qmake feature
# https://stackoverflow.com/a/54162789/1850429
# https://codereview.qt-project.org/#/c/156784/
CONFIG += file_copies

QT += core gui
QT += serialport
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MultiTerminal
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    filtermanager.cpp \
    lineassembler_class.cpp \
    view.cpp \
    viewmanager.cpp \
    about.cpp \
    config.cpp \
    drivermanager.cpp

HEADERS  += mainwindow.h \
    filtermanager.h \
    lineassembler_class.h \
    view.h \
    viewmanager.h \
    about.h \
    config.h \
    drivermanager.h \
    ascii.h

FORMS    += mainwindow.ui \
    view.ui \
    about.ui \
    config.ui

OTHER_FILES += \
    BUGS.txt \
    TODO.txt \
    icon.rc

RESOURCES += \
    resources.qrc

# Deployment script
deployscript.files = $$files($$PWD/../windows/deploy.bat)
deployscript.path = $$OUT_PWD
CONFIG(release, debug|release) {
    #message( "execute deploy.bat in Qt 5.x.x Shell when in build directory to collect all dependencies" )
    COPIES += deployscript
    # Execute deployscript on rebuild. (does not work on incremental builds, only clean builds)
    QMAKE_POST_LINK += $$OUT_PWD/deploy.bat
}

# Installer script (inno setup)
installscript.files = $$files($$PWD/../windows/installer.iss)
installscript.path = $$OUT_PWD
CONFIG(release, debug|release) {
    #message( "execute installer.iss with Inno Setup Creator 6 from build directory to create installer" )
    # this is executed from the deployscript.bat
    COPIES += installscript
}
