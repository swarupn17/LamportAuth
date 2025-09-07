#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "../network/NetworkManager.h"
#include "../app/Controller.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // create components
    m_network = new NetworkManager(this);
    m_controller = new Controller(this);

    // wire controller to network
    m_controller->setNetworkManager(m_network);

    // connect UI buttons
    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::onBtnConnectClicked);
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::onBtnStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::onBtnStopClicked);
    connect(ui->btnDisconnect, &QPushButton::clicked, this, &MainWindow::onBtnDisconnectClicked);

    // wire Controller signals to UI
    connect(m_controller, &Controller::logMessage, this, &MainWindow::appendLog);
    connect(m_controller, &Controller::statusChanged, this, &MainWindow::onStatusChanged);
    connect(m_controller, &Controller::roundChanged, this, &MainWindow::onRoundChanged);

    // initial UI state
    ui->btnStart->setEnabled(false);
    ui->btnDisconnect->setEnabled(true); 
    setStatus("Idle");
    ui->lblRound->setText("Round: 0 / 0");
}

// Appends to the QTextEdit (log) with color for PASS/FAIL
void MainWindow::appendLog(const QString &line)
{
    // determine color by keywords
    QString lower = line.toLower();
    QString htmlLine = line.toHtmlEscaped();
    if (lower.contains("verify ok") || lower.contains("sent response") || lower.contains("sent challenge")) {
        // green
        ui->txtLog->append(QString("<div style='color:green;'>%1</div>").arg(htmlLine));
    } else if (lower.contains("failed") || lower.contains("verification failed") || lower.contains("error")) {
        // red
        ui->txtLog->append(QString("<div style='color:red;'>%1</div>").arg(htmlLine));
    } else {
        // default (black)
        ui->txtLog->append(QString("<div style='color:black;'>%1</div>").arg(htmlLine));
    }
}

// Update the status label
void MainWindow::onStatusChanged(const QString &status)
{
    setStatus(status);

    if (status.compare("Connected", Qt::CaseInsensitive) == 0) {
    ui->btnStart->setEnabled(true);
    ui->btnConnect->setEnabled(false);
} else if (status.compare("Disconnected", Qt::CaseInsensitive) == 0 ||
           status.compare("Stopped", Qt::CaseInsensitive) == 0 ||
           status.compare("Finished", Qt::CaseInsensitive) == 0 ||
           status.compare("Error", Qt::CaseInsensitive) == 0) {
    ui->btnStart->setEnabled(false);
    ui->btnConnect->setEnabled(true);
} else if (status.compare("Running", Qt::CaseInsensitive) == 0) {
    ui->btnStart->setEnabled(false);
    ui->btnConnect->setEnabled(false);
} else if (status.compare("Listening", Qt::CaseInsensitive) == 0) {
    ui->btnStart->setEnabled(false);
    ui->btnConnect->setEnabled(false);
}

// keep Disconnect always enabled
ui->btnDisconnect->setEnabled(true);

}

void MainWindow::onRoundChanged(int current, int total)
{
    ui->lblRound->setText(QString("Round: %1 / %2").arg(current).arg(total));
}

void MainWindow::onBtnConnectClicked()
{
    QString cfgPath = QFileDialog::getOpenFileName(this, "Select config file", QString(), "JSON Files (*.json)");
    if (cfgPath.isEmpty()) return;

    QString err;
    if (!m_controller->loadConfig(cfgPath, err)) {
        QMessageBox::critical(this, "Config load error", err);
        return;
    }

    // Decide behavior by role: Bob listens, Alice connects
    QString role = m_controller->role();
    if (role.compare("Bob", Qt::CaseInsensitive) == 0) {
        QString netErr;
        bool ok = m_controller->startServer(netErr);
        if (!ok) {
            appendLog(QString("Failed to start server: %1").arg(netErr));
            QMessageBox::warning(this, "Network", netErr);
            return;
        }
        appendLog(QString("Server started and listening (role=Bob)"));
        // set status to Listening (Controller will also emit)
        onStatusChanged("Listening");
    } else {
        // Default: Alice tries to connect as client
        QString netErr;
        m_controller->connectToPeer(netErr);
        if (!netErr.isEmpty()) {
            appendLog(QString("Connect (client) error: %1").arg(netErr));
            QMessageBox::warning(this, "Network", netErr);
            return;
        }
        appendLog("Attempting to connect as client...");
        // waiting for controller to emit Connected
    }
}

void MainWindow::onBtnStartClicked()
{
    // Start the protocol (Alice will begin sending challenges; Bob will just wait)
    m_controller->start();
}

void MainWindow::onBtnStopClicked()
{
    m_controller->stop();
}

void MainWindow::onBtnDisconnectClicked()
{
    // disconnect network and update UI
    m_network->disconnectFromPeer();
    // if server was started, stop server as well
    m_controller->stop();
    appendLog("Disconnect requested.");
    onStatusChanged("Disconnected");
}

void MainWindow::setStatus(const QString &s)
{
    ui->lblStatus->setText(QString("Status: %1").arg(s));
}

MainWindow::~MainWindow()
{
    delete ui;
}
