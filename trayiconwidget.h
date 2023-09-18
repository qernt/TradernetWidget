#ifndef TRAYICONWIDGET_H
#define TRAYICONWIDGET_H

#include <QWidget>

namespace Ui {
class trayIconWidget;
}

class trayIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit trayIconWidget(QWidget *parent = nullptr);
    ~trayIconWidget();

private:
    Ui::trayIconWidget *ui;
};

#endif // TRAYICONWIDGET_H
