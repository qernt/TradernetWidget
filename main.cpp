#include "mainwindow.h"
#include "trayiconwidget.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QLabel>
#include <QGridLayout>
#include <QWidgetAction>
#include <Python.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Py_Initialize();

    MainWindow mainWindow;

    trayIconWidget* trayIconW = new trayIconWidget();

    QSystemTrayIcon trayIcon(QIcon("faviconV2.png"));
    trayIcon.show();

    QMenu trayMenu;

    QWidgetAction* widgetAction = new QWidgetAction(nullptr);
    widgetAction->setDefaultWidget(trayIconW);

    trayMenu.addAction(widgetAction);

    trayIcon.setContextMenu(&trayMenu);

    return a.exec();
}
