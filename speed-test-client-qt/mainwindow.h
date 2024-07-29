#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void downloadCompleted();
    void uploadCompleted();

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
    QTimer *timer;
    QDateTime startTime;
    qint64 downloadSize;
    qint64 uploadSize;
    qint64 totalUploadedSize;
    const qint64 maxUploadSize;
};

#endif // MAINWINDOW_H
