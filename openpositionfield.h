#ifndef OPENPOSITIONFIELD_H
#define OPENPOSITIONFIELD_H

#include <QWidget>

namespace Ui {
class OpenPositionField;
}

class OpenPositionField : public QWidget
{
    Q_OBJECT

public:
    explicit OpenPositionField(QWidget *parent = nullptr, QString ticker = "", QString value = "", QString forYesterday = "", QString forAnEntierPeriod = "", QString currency = "");
    ~OpenPositionField();

private:
    Ui::OpenPositionField *ui;

public:
//    void updatePositinInformation(QString newData[3]);
};

#endif // OPENPOSITIONFIELD_H
