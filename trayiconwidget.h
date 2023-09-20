#ifndef TRAYICONWIDGET_H
#define TRAYICONWIDGET_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class TrayIconWidget;
}

class TrayIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrayIconWidget(QWidget *parent = nullptr);
    ~TrayIconWidget();

    QString getPublicKey() const;
    void setPublicKey();

    QString getPrivateKey() const;
    void setPrivateKey();

private:
    void requestToAPI();

private:
    QString publicKey;
    QString privateKey;

public:
    QSettings* settings;

private:
    Ui::TrayIconWidget *ui;
};

#endif // TRAYICONWIDGET_H
