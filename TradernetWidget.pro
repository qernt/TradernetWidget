QT       += core gui
QT += core gui network
LIBS += -lcurl
LIBS += -L$$PWD/../../../opt/homebrew/Cellar/curl/8.0.1/include



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    settingswidget.cpp \
    trayiconwidget.cpp

HEADERS += \
    json.hpp \
    settingswidget.h \
    trayiconwidget.h

FORMS += \
    settingswidget.ui \
    trayiconwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    requestScript.py


