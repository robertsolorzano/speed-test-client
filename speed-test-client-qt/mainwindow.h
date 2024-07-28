#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
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
    void downloadButtonClicked();
    void uploadButtonClicked();
    void latencyButtonClicked();
    void handleNetworkData(QNetworkReply *networkReply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *networkReply;
    qint64 downloadSize;
    qint64 uploadSize;
    QDateTime startTime;
    QTimer *timer;

    void startDownload();
    void updateDownloadProgress();
    void calculateDownloadSpeed();
    void finalizeDownloadSpeed();

    void startUpload();
    void updateUploadProgress();
    void calculateUploadSpeed();
    void finalizeUploadSpeed();
};

#endif // MAINWINDOW_H
