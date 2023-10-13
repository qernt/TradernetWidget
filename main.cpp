#include "trayiconwidget.h"
#include "settingswidget.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QWidgetAction>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TrayIconWidget* trayIconWidget = new TrayIconWidget();

    SettingsWidget* settings = new SettingsWidget();

    QSystemTrayIcon trayIcon(QIcon("/Users/alexeygolubev/Documents/programming/TradernetWidget/FRHC-19d5c74f.png"));
    trayIcon.show();

    QMenu trayMenu;

    trayMenu.setStyleSheet("QMenu::item { width: 20px; }");

    QWidgetAction* widgetAction = new QWidgetAction(nullptr);
    widgetAction->setDefaultWidget(trayIconWidget);

    trayMenu.addAction(widgetAction);

    trayIcon.setContextMenu(&trayMenu);

    trayMenu.addSeparator();

    QAction showSettingsWindow("Settings", &a);
    QObject::connect(&showSettingsWindow, &QAction::triggered, [&settings](){
        settings->show();
    });
    trayMenu.addAction(&showSettingsWindow);

    QAction quitAction("Quit", &a);
    QObject::connect(&quitAction, &QAction::triggered, &a, &QApplication::quit);
    trayMenu.addAction(&quitAction);

    trayMenu.update();

    return a.exec();
}
