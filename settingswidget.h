#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

protected:
    void closeEvent(QCloseEvent *event) override
    {
        event->ignore();
        hide();
    }

private:
    Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
