#include "trayiconwidget.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QWidgetAction>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TrayIconWidget* trayIconWidget = new TrayIconWidget();

    QSystemTrayIcon trayIcon(QIcon("faviconV2.png"));
    trayIcon.show();

    QMenu trayMenu;

    QWidgetAction* widgetAction = new QWidgetAction(nullptr);
    widgetAction->setDefaultWidget(trayIconWidget);

    trayMenu.addAction(widgetAction);

    trayIcon.setContextMenu(&trayMenu);

    return a.exec();
}
