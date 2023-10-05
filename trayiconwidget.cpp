#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

#include "settingswidget.h"
#include "openpositionfield.h"

#include <unistd.h>
#include <QProcess>
#include <QTimer>
#include <QThread>
#include <thread>
#include "json.hpp"

TrayIconWidget::TrayIconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrayIconWidget)
{
    ui->setupUi(this);

    QWidget *scrollWidget = new QWidget();

    ui->scrollArea->setWidget(scrollWidget);

    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setAlignment(Qt::AlignTop);

    settings = new QSettings("AG", "TradernetWidget");

    std::thread requestThread(std::bind(&TrayIconWidget::loadDataFromAPI, this));
    requestThread.detach();


    connect(this, SIGNAL(newPosition(QString,QString,QString,QString,QString)), this, SLOT(createNewPole(QString,QString,QString,QString,QString)));

}

TrayIconWidget::~TrayIconWidget()
{
    delete ui;
}

QByteArray TrayIconWidget::requestToAPI()
{
    QProcess pythonProcess;
    pythonProcess.setProgram("/opt/homebrew/bin/python3");

    QStringList arguments;

    arguments << "/Users/alexeygolubev/Documents/programming/TradernetWidget/requestScript.py" << settings->value("PublicKey").toString() << settings->value("PrivateKey").toString();
    pythonProcess.setArguments(arguments);

    pythonProcess.start();

    if (!pythonProcess.waitForStarted()) {
        qDebug() << "Failed to start Python process";
        return NULL;
    }

    if (!pythonProcess.waitForFinished(-1)) {
        qDebug() << "Python script did not finish successfully";
        return NULL;
    }

    pythonProcess.kill();

    int exitCode = pythonProcess.exitCode();
    if (exitCode == 0) {
        qDebug() << "Python script executed successfully";
        return pythonProcess.readAll();
    }else{
        qDebug() << "Python script exited with an error code: " << exitCode;
        return NULL;
    }
}

std::vector<std::vector<QString>> TrayIconWidget::parsingJSON(QByteArray JSON)
{
    try{
        std::string response = JSON.constData();
        nlohmann::json data = nlohmann::json::parse(response);
        std::vector<std::vector<QString>> parcedData;

        if (data.contains("result") && data["result"].contains("ps") && data["result"]["ps"].contains("pos")) {
            for (const auto& pos_item : data["result"]["ps"]["pos"]) {
                std::vector<QString> position = {QString::fromStdString(pos_item["i"]), QString::fromStdString(pos_item["curr"]), QString::number(static_cast<float>(pos_item["market_value"])),
                                                 QString::number(static_cast<float>(pos_item["profit_close"])), QString::number(static_cast<float>(pos_item["profit_price"]))};
                parcedData.push_back(position);
            }
            return parcedData;
        } else {
            qDebug() << "JSON does not contain expected structure.";
            return parcedData;
        }
    } catch (const std::exception& e) {
        qDebug() << "An exception occurred: " << e.what();
    }
}

void TrayIconWidget::loadDataFromAPI()
{
    QByteArray output = requestToAPI();

    if (!output.isEmpty()) {
        qDebug() << output;
        std::vector<std::vector<QString>> recivedData = parsingJSON(output);
        for (const std::vector<QString>& innerVector : recivedData) {
            emit newPosition(innerVector[0], innerVector[2], innerVector[3], innerVector[3], innerVector[1]);
            qDebug() << "sadasdsad";
        }
    }

    QCoreApplication::processEvents();

    while(true){
        QByteArray output = requestToAPI();

        if (!output.isEmpty()) {
            std::vector<std::vector<QString>> recivedData = parsingJSON(output);
            for (const std::vector<QString>& innerVector : recivedData) {
                for (QString element : innerVector) {
                    qDebug() << element << " ";
                }
                qDebug() << "\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void TrayIconWidget::createNewPole(QString ticker, QString value, QString forYesterday,QString forAnEntierPeriod, QString currency)
{
    OpenPositionField* op = new OpenPositionField(this, ticker, value, forYesterday, forAnEntierPeriod, "122");
    scrollLayout->insertWidget(0, op);
}

//Загружаем нынешние открытые позиции
//Обновляем их
//Если при обновлении появилась новая позиция то создаем новый экземпляр класса

