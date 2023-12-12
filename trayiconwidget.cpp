#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

#include "settingswidget.h"
#include <unistd.h>
#include <QProcess>
#include <thread>
#include <QLabel>
#include <QtNetwork>
#include <QPixmap>
#include "json.hpp"
#include <curl/curl.h>
#include <cmath>

const std::string FINHUB_TOKEN = "YOUR_FINHUB_TOKEN";

struct PositionField{
private:
    QString ticker;
    int quantity;
    float openBalance;
    float forYesterday;

    QLabel *labelLogo;
    QLabel *labelTicker;
    QLabel *labelValue;
    float oldValue;
    QLabel *labelForToday;
    float oldForToday;
    QLabel *labelForYesterday;
    QLabel *labelForAnEntirePeriod;
    float oldForEntirePeriod;

public:
    PositionField(QString Ticker, QLabel *LabelLogo = nullptr,QLabel *LabelTicker = nullptr, QLabel *LabelValue = nullptr, QLabel *LabelForToday = nullptr, QLabel *LabelForYesterday = nullptr, QLabel *LabelForAnEntirePeriod = nullptr, int Quantity = 0, float OpenBalance = 0, float ForYesterday = 0){
        ticker = Ticker;
        labelLogo = LabelLogo;
        labelTicker = LabelTicker;
        labelTicker->setAlignment(Qt::AlignCenter);

        labelValue = LabelValue;
        labelValue->setAlignment(Qt::AlignCenter);

        labelForToday = LabelForToday;
        labelForToday->setAlignment(Qt::AlignCenter);
        labelTextColor(labelForToday->text().toFloat(), labelForToday);

        labelForYesterday = LabelForYesterday;
        labelForYesterday->setAlignment(Qt::AlignCenter);
        labelTextColor(labelForYesterday->text().toFloat(), labelForYesterday);

        labelForAnEntirePeriod = LabelForAnEntirePeriod;
        labelForAnEntirePeriod->setAlignment(Qt::AlignCenter);
        labelTextColor(labelForAnEntirePeriod->text().toFloat(), labelForAnEntirePeriod);

        quantity = Quantity;
        openBalance = OpenBalance;
        forYesterday = ForYesterday;
    }

    void updateData(float value, float forToday, float forAnEntirePeriod){
        labelValue->setText(QString::number(value));
        oldValue = value;

        labelForToday->setText(QString::number(forToday));
        labelTextColor(forToday, labelForToday);
        oldForToday = forToday;

        labelForAnEntirePeriod->setText(QString::number(forAnEntirePeriod));
        labelTextColor(forAnEntirePeriod, labelForAnEntirePeriod);
        oldForEntirePeriod = forAnEntirePeriod;
    }

    void labelTextColor(float newValue, QLabel *label){
        if(this->ticker == "Ticker") return;
        if(newValue >= 0){
            label->setStyleSheet("color: green;");
        }else{
            label->setStyleSheet("color: red;");
        }
    }

    QString getTicker() const{
        return ticker;
    }

    void setTicker(const QString &newTicker){
        ticker = newTicker;
    }

    int getQuantity() const{
        return quantity;
    }
    void setQuantity(int newQuantity){
        quantity = newQuantity;
    }

    float getOpenBalance() const{
        return openBalance;
    }
    void setOpenBalance(float newOpenBalance){
        openBalance = newOpenBalance;
    }

    float getForYesterday() const{
        return forYesterday;
    }
    void setForYesterday(float newForYesterday){
        forYesterday = newForYesterday;
    }
};

std::vector<PositionField*> positionFieldsVector;
int fieldsCount = 0;

float roundFloat(float value)
{
    value = roundf(value * 100) / 100;
    return value;
}


TrayIconWidget::TrayIconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrayIconWidget)
{
    ui->setupUi(this);

    QWidget *scrollWidget = new QWidget();

    ui->scrollArea->setWidget(scrollWidget);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    scrollLayout = new QGridLayout(scrollWidget);
    scrollLayout->setAlignment(Qt::AlignTop);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    createNewPole("Ticker", "Value","For Today", "For Yesterday", "For an entire period", 0, 0, 0);

    settings = new QSettings("AG", "TradernetWidget");

    std::thread requestThread(std::bind(&TrayIconWidget::initializeOpenPositions, this));
    requestThread.detach();

    connect(this, SIGNAL(newPosition(QString,QString,QString,QString,QString,int,float,float)), this, SLOT(createNewPole(QString,QString,QString,QString,QString,int,float,float)));
}

TrayIconWidget::~TrayIconWidget()
{
    delete ui;
}

QByteArray TrayIconWidget::requestToTradernetAPI()
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

std::vector<std::vector<QString>> TrayIconWidget::parseTradernetResponse(QByteArray JSON)
{
    try{
        std::string response = JSON.constData();
        nlohmann::json data = nlohmann::json::parse(response);
        std::vector<std::vector<QString>> parcedData;

        if (data.contains("result") && data["result"].contains("ps") && data["result"]["ps"].contains("pos")) {
            for (const auto& pos_item : data["result"]["ps"]["pos"]) {
                std::vector<QString> position = {QString::fromStdString(pos_item["i"]), QString::number(static_cast<float>(pos_item["open_bal"])),
                                                 QString::number(static_cast<int>(pos_item["q"])), QString::number(static_cast<float>(pos_item["profit_close"]))};
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

float* calculateData(int quantity, float openBalance, float currentPricePerOne, float forYesterday){
    float *calculatedDataArray = new float[3];

    calculatedDataArray[0] = roundFloat(currentPricePerOne * quantity); //value
    calculatedDataArray[2] = roundFloat((currentPricePerOne * quantity) - openBalance); // ForAnEntirePeriod
    calculatedDataArray[1] = roundFloat(calculatedDataArray[2] - forYesterday);  // ForToday

    return calculatedDataArray;
}

float parseFinhubResponse(std::string response)
{
    try {
        nlohmann::json jsonData = nlohmann::json::parse(response);

        float c = jsonData["c"];

        return c;
    } catch (const nlohmann::json::exception& e) {
        qDebug() << "Response parsing error: " << e.what();
        qDebug() << response;
    }
}

void TrayIconWidget::initializeOpenPositions()
{
    QByteArray output = requestToTradernetAPI();

    if (!output.isEmpty()) {
        std::vector<std::vector<QString>> receivedData = parseTradernetResponse(output);
        for (const std::vector<QString>& tradernetDataVector : receivedData) {
            std::string finhubData = getDataAboutPositionsFromFinhub(tradernetDataVector[0].toStdString());

            if(finhubData != ""){
                float parsedData = parseFinhubResponse(finhubData);
                float *calculatedData = calculateData(tradernetDataVector[2].toInt(), tradernetDataVector[1].toFloat(), parsedData, tradernetDataVector[3].toFloat());

                emit newPosition(tradernetDataVector[0], QString::number(calculatedData[0]), QString::number(calculatedData[1]), tradernetDataVector[3], QString::number(calculatedData[2]), tradernetDataVector[2].toInt(), tradernetDataVector[1].toFloat(),  tradernetDataVector[3].toFloat());

                delete[] calculatedData;
            }
            QCoreApplication::processEvents();
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    upadtePositionsData();
}

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string TrayIconWidget::getDataAboutPositionsFromFinhub(std::string ticker)
{
    size_t found = ticker.find(".US");
    if (found != std::string::npos) {
        ticker.erase(found, 3);
    }

    std::string url = "https://finnhub.io/api/v1/quote?symbol=" + ticker + "&token=" +  FINHUB_TOKEN ;

    CURL *curl;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        return readBuffer;
    }
    return nullptr;
}

void TrayIconWidget::createNewPole(QString ticker, QString value, QString forToday, QString forYesterday, QString forAnEntirePeriod, int quantity, float openBalance, float floatForYesterday)
{
    QLabel *labelLogo = new QLabel("logo");
    QLabel *labelTicker = new QLabel(ticker);
    QLabel *labelValue = new QLabel(value);
    QLabel *labelForToday = new QLabel(forToday);
    QLabel *labelForYesterday = new QLabel(forYesterday);
    QLabel *labelForAnEntirePeriod = new QLabel(forAnEntirePeriod);


    labelLogo->setPixmap(getPositionLogo(ticker));
    labelLogo->setScaledContents(true);
    labelLogo->setFixedSize(30,30);

    PositionField *positionFiled = new PositionField(ticker, labelLogo, labelTicker, labelValue, labelForToday, labelForYesterday, labelForAnEntirePeriod, quantity, openBalance, floatForYesterday);

    positionFieldsVector.push_back(positionFiled);

    scrollLayout->addWidget(labelLogo, fieldsCount, 0);
    scrollLayout->addWidget(labelTicker, fieldsCount, 1);
    scrollLayout->addWidget(labelValue, fieldsCount, 2);
    scrollLayout->addWidget(labelForToday, fieldsCount, 3);
    scrollLayout->addWidget(labelForYesterday, fieldsCount, 4);
    scrollLayout->addWidget(labelForAnEntirePeriod, fieldsCount, 5);

    fieldsCount++;

    QCoreApplication::processEvents();
}

QPixmap TrayIconWidget::getPositionLogo(QString ticker)
{
    QPixmap pixmap;
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    ticker = ticker.toLower();
    QString imageURLQString = "https://tradernet.kz/logos/get-logo-by-ticker?ticker=" + ticker;
    QUrl imageURL(imageURLQString);
    QNetworkReply *reply = networkManager->get(QNetworkRequest(imageURL));

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray imageData = reply->readAll();
        pixmap.loadFromData(imageData);
    } else {
        qDebug() << "Error loading image:" << reply->errorString();
    }

    reply->deleteLater();
    return pixmap;
}

void TrayIconWidget::upadtePositionsData()
{
    std::string finhubData;
    float parsedData;
    float *calculatedData;
    while(true){
        for(int i = 1; i < positionFieldsVector.size(); i++){
            finhubData = getDataAboutPositionsFromFinhub(positionFieldsVector[i]->getTicker().toStdString());
            if(finhubData != ""){
                parsedData = parseFinhubResponse(finhubData);
                calculatedData = calculateData(positionFieldsVector[i]->getQuantity(), positionFieldsVector[i]->getOpenBalance(), parsedData, positionFieldsVector[i]->getForYesterday());

                positionFieldsVector[i]->updateData(calculatedData[0], calculatedData[1], calculatedData[2]);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}
