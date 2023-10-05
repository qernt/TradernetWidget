#ifndef TRAYICONWIDGET_H
#define TRAYICONWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QSettings>
#include <mutex>
#include <vector>


namespace Ui {
class TrayIconWidget;
}

class TrayIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrayIconWidget(QWidget *parent = nullptr);
    ~TrayIconWidget();

private:
    QByteArray requestToAPI();
    std::vector<std::vector<QString>> parsingJSON(QByteArray JSON);
    void loadDataFromAPI();

public:
    QSettings* settings;

private slots:
    void createNewPole(QString ticker, QString value, QString forYesterday,QString forAnEntierPeriod, QString currency);

signals:
    void newPosition(QString ticker, QString value, QString forYesterday,QString forAnEntierPeriod, QString currency);

private:
    Ui::TrayIconWidget *ui;
    QVBoxLayout *scrollLayout;
};

#endif // TRAYICONWIDGET_H
