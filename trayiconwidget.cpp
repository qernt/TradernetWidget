#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

#include "settingswidget.h"

#include <unistd.h>
#include <QProcess>
#include <QTimer>
#include <thread>
#include "json.hpp"

TrayIconWidget::TrayIconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrayIconWidget)
{
    ui->setupUi(this);

    settings = new QSettings("AG", "TradernetWidget");

    setPublicKey();
    setPrivateKey();

    std::thread requestThread(std::bind(&TrayIconWidget::requestToAPI, this));
    requestThread.detach();
}

TrayIconWidget::~TrayIconWidget()
{
    delete ui;
}

void TrayIconWidget::requestToAPI()
{
    QProcess pythonProcess;
    pythonProcess.setProgram("/opt/homebrew/bin/python3");

    QStringList arguments;
    arguments << "/Users/alexeygolubev/Documents/programming/TradernetWidget/requestScript.py" << getPublicKey() << getPrivateKey();

    pythonProcess.setArguments(arguments);

    while(true){
        pythonProcess.start();

        if (!pythonProcess.waitForStarted()) {
            qDebug() << "Failed to start Python process";
            return;
        }

        if (!pythonProcess.waitForFinished(-1)) {
            qDebug() << "Python script did not finish successfully";
            return;
        }

        QByteArray output = pythonProcess.readAll();

        std::string response = output.constData();


        nlohmann::json data = nlohmann::json::parse(response);

        for (const auto& pos_item : data["result"]["ps"]["pos"]) {
            std::string i = pos_item["i"];
            int t = pos_item["open_bal"];
            qDebug() << "i: " << i << ", t: " << t;
        }

        int exitCode = pythonProcess.exitCode();
        if (exitCode == 0) {
            qDebug() << "Python script executed successfully";
        } else {
            qDebug() << "Python script exited with an error code: " << exitCode;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void TrayIconWidget::setPrivateKey()
{
    privateKey = settings->value("PrivateKey").toString();;
}

QString TrayIconWidget::getPrivateKey() const
{
    return privateKey;
}

void TrayIconWidget::setPublicKey()
{
    publicKey = settings->value("PublicKey").toString();
}

QString TrayIconWidget::getPublicKey() const
{
    return publicKey;
}

