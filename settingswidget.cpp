#include "settingswidget.h"
#include "ui_settingswidget.h"
#include <QSettings>

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    QSettings settings("AG", "TradernetWidget");

    ui->lineEdit->setText(settings.value("PublicKey").toString());
    ui->lineEdit_2->setText(settings.value("PrivateKey").toString());
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::on_pushButton_clicked()
{
    QSettings settings("AG", "TradernetWidget");

    settings.setValue("PublicKey", ui->lineEdit->text());
    settings.setValue("PrivateKey", ui->lineEdit_2->text());
}

