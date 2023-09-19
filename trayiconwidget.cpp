#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

#include "settingswidget.h"

#include <QProcess>

TrayIconWidget::TrayIconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrayIconWidget)
{
    ui->setupUi(this);

    QString publicKey = "";
    QString privateKey = "";

    QProcess pythonProcess;
    pythonProcess.setProgram("/opt/homebrew/bin/python3");

    QStringList arguments;
    arguments << "/Users/alexeygolubev/Documents/programming/TradernetWidget/requestScript.py" << publicKey << privateKey;

    pythonProcess.setArguments(arguments);
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

    qDebug() << "Python script output:" << output;

    int exitCode = pythonProcess.exitCode();
    if (exitCode == 0) {
        qDebug() << "Python script executed successfully";
    } else {
        qDebug() << "Python script exited with an error code: " << exitCode;
    }

    settingsWidget = new SettingsWidget();
}

TrayIconWidget::~TrayIconWidget()
{
    delete ui;
}


void TrayIconWidget::on_pushButton_clicked()
{
    settingsWidget->show();
}

