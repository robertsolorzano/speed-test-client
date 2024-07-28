#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
    , networkReply(nullptr)
    , downloadSize(0)
    , uploadSize(0)
{
    ui->setupUi(this);

    connect(ui->testMySpeedButton, &QPushButton::clicked, this, &MainWindow::testMySpeed);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleNetworkData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testMySpeed()
{
    qDebug() << "Test My Speed button clicked";

    startDownload();
}

void MainWindow::startDownload()
{
    qDebug() << "Starting download";

    downloadSize = 0;
    QNetworkRequest request(QUrl("http://localhost:3000/download"));
    networkReply = networkManager->get(request);
    connect(networkReply, &QNetworkReply::readyRead, this, &MainWindow::updateDownloadProgress);
    connect(networkReply, &QNetworkReply::finished, this, &MainWindow::finalizeDownloadSpeed);

    startTime = QDateTime::currentDateTime();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::calculateDownloadSpeed);
    timer->start(1000); // Update speed every second

    qDebug() << "Download request sent, timer started";
}

void MainWindow::updateDownloadProgress()
{
    qDebug() << "Updating download progress";

    downloadSize += networkReply->bytesAvailable();
    networkReply->readAll(); // Read the data to clear the buffer
    qDebug() << "Download size updated to" << downloadSize;
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
    qDebug() << "Finalizing download speed";

    timer->stop();
    qint64 elapsedTime = startTime.msecsTo(QDateTime::currentDateTime());
    double elapsedTimeInSeconds = elapsedTime / 1000.0;
    double mbps = (downloadSize * 8) / elapsedTimeInSeconds / (1024 * 1024); // Convert bytes to Mbps

    ui->downloadSpeedLabel->setText(QString("Final Download Speed: %1 Mbps").arg(mbps, 0, 'f', 2));

    qDebug() << "Download completed, starting upload";
    startUpload();
}

void MainWindow::startUpload()
{
    qDebug() << "Starting upload";

    uploadSize = 0;
    QByteArray data;
    data.resize(1024 * 1024); // 1MB chunk
    data.fill('a');

    QNetworkRequest request(QUrl("http://localhost:3000/upload"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");

    networkReply = networkManager->post(request, data);
    connect(networkReply, &QNetworkReply::readyRead, this, &MainWindow::updateUploadProgress);
    connect(networkReply, &QNetworkReply::finished, this, &MainWindow::finalizeUploadSpeed);

    startTime = QDateTime::currentDateTime();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::calculateUploadSpeed);
    timer->start(1000); // Update speed every second

    qDebug() << "Upload request sent, timer started";
}

void MainWindow::updateUploadProgress()
{
    qDebug() << "Updating upload progress";

    QByteArray responseData = networkReply->readAll();
    qDebug() << "Response Data: " << responseData;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    uploadSize = jsonObj["size"].toInt();
    double mbps = jsonObj["mbps"].toString().toDouble(); // Ensure mbps is correctly converted to double

    ui->uploadSpeedLabel->setText(QString("Upload Speed: %1 Mbps").arg(mbps, 0, 'f', 2));
    qDebug() << "Upload size updated to" << uploadSize << ", Mbps: " << mbps;
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
    qDebug() << "Finalizing upload speed";

    timer->stop();
    qint64 elapsedTime = startTime.msecsTo(QDateTime::currentDateTime());
    double elapsedTimeInSeconds = elapsedTime / 1000.0;
    double mbps = (uploadSize * 8) / elapsedTimeInSeconds / (1024 * 1024); // Convert bytes to Mbps

    ui->uploadSpeedLabel->setText(QString("Final Upload Speed: %1 Mbps").arg(mbps, 0, 'f', 2));

    qDebug() << "Upload completed, starting latency test";
    startLatency();
}

void MainWindow::startLatency()
{
    qDebug() << "Starting latency test";

    QNetworkRequest request(QUrl("http://localhost:3000/latency"));
    networkManager->get(request);
}

void MainWindow::handleNetworkData(QNetworkReply *networkReply)
{
    if (networkReply->error() == QNetworkReply::NoError) {
        QByteArray responseData = networkReply->readAll();
        qDebug() << "Received Data: " << responseData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        QString message = jsonObj["message"].toString();
        qint64 serverTimestamp = jsonObj["serverTimestamp"].toVariant().toLongLong();
        qint64 serverProcessingTime = jsonObj["serverProcessingTime"].toVariant().toLongLong();

        // Convert the server timestamp to a readable format
        QDateTime dateTime;
        dateTime.setMSecsSinceEpoch(serverTimestamp);
        QString formattedTime = dateTime.toString("hh:mm:ss AP");

        QString result = QString("Message: %1\nServer Time: %2\nServer Processing Time: %3 ms")
                             .arg(message, formattedTime)
                             .arg(serverProcessingTime);

        ui->resultLabel->setText(result);
    } else {
        ui->resultLabel->setText("Error: " + networkReply->errorString());
    }

    networkReply->deleteLater();
}

