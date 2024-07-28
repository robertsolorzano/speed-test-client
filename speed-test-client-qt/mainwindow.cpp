#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
    , networkReply(nullptr)
    , downloadSize(0)
    , uploadSize(0)
{
    ui->setupUi(this);

    connect(ui->downloadButton, &QPushButton::clicked, this, &MainWindow::downloadButtonClicked);
    connect(ui->uploadButton, &QPushButton::clicked, this, &MainWindow::uploadButtonClicked);
    connect(ui->latencyButton, &QPushButton::clicked, this, &MainWindow::latencyButtonClicked);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleNetworkData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::downloadButtonClicked()
{
    startDownload();
}

void MainWindow::uploadButtonClicked()
{
    startUpload();
}

void MainWindow::latencyButtonClicked()
{
    QNetworkRequest request(QUrl("http://localhost:3000/latency"));
    networkManager->get(request);
}

void MainWindow::startDownload()
{
    downloadSize = 0;
    QNetworkRequest request(QUrl("http://localhost:3000/download"));
    networkReply = networkManager->get(request);
    connect(networkReply, &QNetworkReply::readyRead, this, &MainWindow::updateDownloadProgress);
    connect(networkReply, &QNetworkReply::finished, this, &MainWindow::finalizeDownloadSpeed);

    startTime = QDateTime::currentDateTime();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::calculateDownloadSpeed);
    timer->start(1000); // Update speed every second
}

void MainWindow::updateDownloadProgress()
{
    downloadSize += networkReply->bytesAvailable();
    networkReply->readAll(); // Read the data to clear the buffer
}

void MainWindow::calculateDownloadSpeed()
{
    qint64 elapsedTime = startTime.msecsTo(QDateTime::currentDateTime());
    double elapsedTimeInSeconds = elapsedTime / 1000.0;
    double mbps = (downloadSize * 8) / elapsedTimeInSeconds / (1024 * 1024); // Convert bytes to Mbps

    ui->downloadSpeedLabel->setText(QString("Download Speed: %1 Mbps").arg(mbps, 0, 'f', 2));
}

void MainWindow::finalizeDownloadSpeed()
{
    timer->stop();
    qint64 elapsedTime = startTime.msecsTo(QDateTime::currentDateTime());
    double elapsedTimeInSeconds = elapsedTime / 1000.0;
    double mbps = (downloadSize * 8) / elapsedTimeInSeconds / (1024 * 1024); // Convert bytes to Mbps

    ui->downloadSpeedLabel->setText(QString("Final Download Speed: %1 Mbps").arg(mbps, 0, 'f', 2));
}

void MainWindow::startUpload()
{
    uploadSize = 0;
    QByteArray data;
    data.resize(1024 * 1024); // 1MB chunk
    data.fill('a');

    QNetworkRequest request(QUrl("http://localhost:3000/upload"));
    networkReply = networkManager->post(request, data);
    connect(networkReply, &QNetworkReply::readyRead, this, &MainWindow::updateUploadProgress);
    connect(networkReply, &QNetworkReply::finished, this, &MainWindow::finalizeUploadSpeed);

    startTime = QDateTime::currentDateTime();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::calculateUploadSpeed);
    timer->start(1000); // Update speed every second
}

void MainWindow::updateUploadProgress()
{
    QByteArray responseData = networkReply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    uploadSize = jsonObj["size"].toInt();
    double mbps = jsonObj["mbps"].toDouble();

    ui->uploadSpeedLabel->setText(QString("Upload Speed: %1 Mbps").arg(mbps, 0, 'f', 2));
}

void MainWindow::calculateUploadSpeed()
{
    qint64 elapsedTime = startTime.msecsTo(QDateTime::currentDateTime());
    double elapsedTimeInSeconds = elapsedTime / 1000.0;
    double mbps = (uploadSize * 8) / elapsedTimeInSeconds / (1024 * 1024); // Convert bytes to Mbps

    ui->uploadSpeedLabel->setText(QString("Upload Speed: %1 Mbps").arg(mbps, 0, 'f', 2));
}

void MainWindow::finalizeUploadSpeed()
{
    timer->stop();
    qint64 elapsedTime = startTime.msecsTo(QDateTime::currentDateTime());
    double elapsedTimeInSeconds = elapsedTime / 1000.0;
    double mbps = (uploadSize * 8) / elapsedTimeInSeconds / (1024 * 1024); // Convert bytes to Mbps

    ui->uploadSpeedLabel->setText(QString("Final Upload Speed: %1 Mbps").arg(mbps, 0, 'f', 2));
}

void MainWindow::handleNetworkData(QNetworkReply *networkReply)
{
    if (networkReply->error() == QNetworkReply::NoError) {
        QByteArray responseData = networkReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        QString message = jsonObj["message"].toString();
        qint64 serverTimestamp = jsonObj["serverTimestamp"].toVariant().toLongLong();
        qint64 serverProcessingTime = jsonObj["serverProcessingTime"].toVariant().toLongLong();

        QString result = QString("Message: %1\nServer Timestamp: %2\nServer Processing Time: %3 ms")
                             .arg(message)
                             .arg(serverTimestamp)
                             .arg(serverProcessingTime);

        ui->resultLabel->setText(result);
    } else {
        ui->resultLabel->setText("Error: " + networkReply->errorString());
    }

    networkReply->deleteLater();
}
