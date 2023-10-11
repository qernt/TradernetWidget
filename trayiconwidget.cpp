#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

#include "settingswidget.h"
#include <unistd.h>
#include <QProcess>
#include <QTimer>
#include <thread>
#include <QLabel>
#include <QtNetwork>
#include <QPixmap>
#include "json.hpp"
#include <curl/curl.h>
#include <cmath>


struct PositionField{
    QString ticker;
    int quantity;

    QLabel *labelLogo;
    QLabel *labelTicker;
    QLabel *labelValue;
    QLabel *labelForToday;
    QLabel *labelForYessterday;
    QLabel *labelForAnEntierPeriod;

    PositionField(QString Ticker, QLabel *LabelLogo = nullptr,QLabel *LabelTicker = nullptr, QLabel *LabelValue = nullptr, QLabel *LabelForToday = nullptr, QLabel *LabelForYessterday = nullptr, QLabel *LabelForAnEntierPeriod = nullptr, int Quantity = 0){
        ticker = Ticker;
        labelLogo = LabelLogo;
        labelTicker = LabelTicker;
        labelValue = LabelValue;
        labelForToday = LabelForToday;
        labelForYessterday = LabelForYessterday;
        labelForAnEntierPeriod = LabelForAnEntierPeriod;
    }
};

std::vector<PositionField*> positionFieldsVector;
int fieldsCount = 0;

float roundFloat(float value){
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

    createNewPole("Ticker", "Value","For Today", "For Yesterday", "For an entire period");

    settings = new QSettings("AG", "TradernetWidget");

    std::thread requestThread(std::bind(&TrayIconWidget::initializeOpenPositions, this));
    requestThread.detach();

    connect(this, SIGNAL(newPosition(QString,QString,QString,QString,QString)), this, SLOT(createNewPole(QString,QString,QString,QString,QString)));
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

float* calculateData(int quantity, float openBalance, float currentPricePerOne, float openPrice){
    float *calculatedDataArray = new float[3];

    calculatedDataArray[0] = roundFloat(currentPricePerOne * quantity);
    calculatedDataArray[1] = roundFloat((currentPricePerOne - openPrice) * quantity);
    calculatedDataArray[2] = roundFloat((currentPricePerOne * quantity) - openBalance);

    return calculatedDataArray;
}

void TrayIconWidget::initializeOpenPositions()
{
    QByteArray output = requestToTradernetAPI();

    QCoreApplication::processEvents();

    if (!output.isEmpty()) {
        std::vector<std::vector<QString>> receivedData = parseTradernetResponse(output);
        for (const std::vector<QString>& tradernetDataVector : receivedData) {
            std::string finhubData = getDataAboutPositionsFromFinhub(tradernetDataVector[0].toStdString());

            if(finhubData != ""){
                std::vector<float> parsedData = parseFinhubResponse(finhubData);
                float *calculatedData = calculateData(tradernetDataVector[2].toFloat(), tradernetDataVector[1].toFloat(), parsedData[0], parsedData[1]);

                emit newPosition(tradernetDataVector[0], QString::number(calculatedData[0]), QString::number(calculatedData[1]), tradernetDataVector[3], QString::number(calculatedData[2]));

                delete[] calculatedData;
            }
        }
    }
}

std::vector<float> TrayIconWidget::parseFinhubResponse(std::string response)
{
    std::vector <float> parsedData;
    try {
        nlohmann::json jsonData = nlohmann::json::parse(response);

        float c = jsonData["c"];
        float o = jsonData["o"];

        parsedData.push_back(c);
        parsedData.push_back(o);

        return parsedData;
    } catch (const nlohmann::json::exception& e) {
        qDebug() << "Response parsing error: " << e.what();
        qDebug() << response;
    }
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

    std::string url = "https://finnhub.io/api/v1/quote?symbol=" + ticker + "&token=" + "PUT_YOUR_FINHUB_API_TOKEN"; //PUT_YOUR_FINHUB_API_TOKEN

    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        return readBuffer;
    }
}

void TrayIconWidget::createNewPole(QString ticker, QString value, QString forToday, QString forYesterday, QString forAnEntirePeriod)
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

    PositionField *positionFiled = new PositionField(ticker, labelLogo, labelTicker, labelValue, labelForToday, labelForYesterday, labelForAnEntirePeriod);
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

QPixmap TrayIconWidget::getPositionLogo(QString ticker){
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
