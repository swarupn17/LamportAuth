#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class NetworkManager;
class Controller;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBtnConnectClicked();
    void onBtnStartClicked();
    void onBtnStopClicked();
    void onBtnDisconnectClicked();

    // connected to Controller signals
    void appendLog(const QString &line);
    void onStatusChanged(const QString &status);
    void onRoundChanged(int current, int total);

private:
    Ui::MainWindow *ui;

    NetworkManager *m_network = nullptr;
    Controller *m_controller = nullptr;

    void setupConnections();
    void setStatus(const QString &s);
};

#endif // MAINWINDOW_H
