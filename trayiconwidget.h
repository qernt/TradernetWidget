#ifndef TRAYICONWIDGET_H
#define TRAYICONWIDGET_H

#include <QWidget>

#include "settingswidget.h"


namespace Ui {
class TrayIconWidget;
}

class TrayIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrayIconWidget(QWidget *parent = nullptr);
    ~TrayIconWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::TrayIconWidget *ui;
    SettingsWidget *settingsWidget;
};

#endif // TRAYICONWIDGET_H
