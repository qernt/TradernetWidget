QT       += core gui

INCLUDEPATH += /opt/homebrew/opt/python@3.11/Frameworks/Python.framework/Versions/3.11/include/python3.11
LIBS += -L/opt/homebrew/opt/python@3.11/Frameworks/Python.framework/Versions/3.11/lib/python3.11/config-3.11-darwin -lpython3.11

CONFIG += link_pkgconfig
CONFIG += no_keywords

LIBS += $$QMAKE_PKGCONFIG_LIBS
INCLUDEPATH += $$QMAKE_PKGCONFIG_INCDIR


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    trayiconwidget.cpp

HEADERS += \
    mainwindow.h \
    trayiconwidget.h

FORMS += \
    mainwindow.ui \
    trayiconwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
