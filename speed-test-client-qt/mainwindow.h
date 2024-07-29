#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void testMySpeed();
    void startDownload();
    void updateDownloadProgress();
    void calculateDownloadSpeed();
    void finalizeDownloadSpeed();
    void startUpload();
    void updateUploadProgress();
    void calculateUploadSpeed();
    void finalizeUploadSpeed();
    void startLatency();
    void handleNetworkData(QNetworkReply *networkReply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *networkReply;
    qint64 downloadSize;
    qint64 uploadSize;
    qint64 totalUploadedSize;
    qint64 maxUploadSize;
    QDateTime startTime;
    QTimer *timer;

    void loadEnvFile(const QString &filePath);
};

#endif // MAINWINDOW_H
