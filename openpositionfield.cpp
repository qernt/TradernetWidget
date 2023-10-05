#include "openpositionfield.h"
#include "ui_openpositionfield.h"

#include <QtNetwork>
#include <QPixmap>

OpenPositionField::OpenPositionField(QWidget *parent, QString ticker, QString value, QString forYesterday, QString forAnEntierPeriod, QString currency) :
    QWidget(parent),
    ui(new Ui::OpenPositionField)
{
    ui->setupUi(this);

    ui->label_ticker->setText(ticker);
    ui->label_value->setText(value);
    ui->label_forYesterday->setText(forYesterday);
//    ui->label_forToday->setText(forToday);
    ui->label_forAnEntierPeriod->setText(forAnEntierPeriod);

    ui->label_logo->setFixedSize(40,40);
    ui->label_logo->setScaledContents(true);

    ticker = ticker.toLower();


    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QString imageURLQString = "https://tradernet.kz/logos/get-logo-by-ticker?ticker=" + ticker;
    QUrl imageURL(imageURLQString);
    QNetworkReply *reply = networkManager->get(QNetworkRequest(imageURL));
    QObject::connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray imageData = reply->readAll();

            QPixmap pixmap;
            pixmap.loadFromData(imageData);

            ui->label_logo->setPixmap(pixmap);
            reply->deleteLater();
        } else {
            qDebug() << "Image download error: " << reply->errorString();
            reply->deleteLater();
        }
    });
}

OpenPositionField::~OpenPositionField()
{
    delete ui;
}

//void OpenPositionField::updatePositinInformation(QString newData[4])
//{
//    ui->label_value->setText(newData[0]);
//    ui->label_forYesterday->setText(newDatap[1]);
//    ui->label_forToday->setText(newData[2]);
//    ui->label_forAnEntierPeriod->setText(newData[3]);
//}
