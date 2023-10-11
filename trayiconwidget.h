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
    QByteArray requestToTradernetAPI();
    std::vector<std::vector<QString>> parseTradernetResponse(QByteArray JSON);
    void initializeOpenPositions();
    std::vector<float> parseFinhubResponse(std::string response);
    std::string getDataAboutPositionsFromFinhub(std::string ticker);
    QPixmap getPositionLogo(QString ticker);

public:
    QSettings* settings;

private slots:
    void createNewPole(QString ticker, QString value,QString forToday, QString forYesterday,QString forAnEntierPeriod);

signals:
    void newPosition(QString ticker, QString value,QString forToday, QString forYesterday,QString forAnEntierPeriod);

private:
    Ui::TrayIconWidget *ui;
    QGridLayout *scrollLayout;
};

#endif // TRAYICONWIDGET_H
