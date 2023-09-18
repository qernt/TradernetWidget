#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

trayIconWidget::trayIconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::trayIconWidget)
{
    ui->setupUi(this);
}

trayIconWidget::~trayIconWidget()
{
    delete ui;
}
