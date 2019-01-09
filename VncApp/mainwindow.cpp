#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include "EeoVncWidget.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->setPlainText("10.0.1.22");
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(connectButtonClick()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(disconnectButtonClick()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(viewOnlyButtonClick()));
    connect(ui->widget, SIGNAL(onResize(int, int)), this, SLOT(resizeSlot(int, int)));
    connect(ui->widget, SIGNAL(onConnected(int)), this, SLOT(connectedSlot(int)));
    connect(ui->widget, SIGNAL(onPwd()), this, SLOT(pwdSlot()));
    connect(ui->widget, SIGNAL(onError(int)), this, SLOT(vncErrorSlot(int)));
    toggleButton();
}

MainWindow::~MainWindow()
{
    ui->widget->closeVnc();
    delete ui;
}

void MainWindow::connectButtonClick()
{
    qDebug("connectButtonClick");
    if (ui->widget->isConnected())
    {
        ui->widget->closeVnc();
    }
    ui->widget->connectVnc(ui->textEdit->toPlainText());
    ui->pushButton->setDisabled(true);
}

void MainWindow::viewOnlyButtonClick()
{
    if (ui->widget->isConnected())
    {
        ui->widget->setVncViewOnly(!ui->widget->getVncViewOnly());
    }
}

void MainWindow::disconnectButtonClick()
{
    qDebug("disconnectButtonClick");
    if (ui->widget->isConnected())
    {
        ui->widget->closeVnc();
        toggleButton(false);
    }
}

void MainWindow::resizeSlot(int x, int y)
{
    qDebug() << "MainWindow::resizeSlot" << QThread::currentThreadId();
    resize(x, y);
}

void MainWindow::connectedSlot(int connectResult)
{
    qDebug() << "MainWindow::connectedSlot:" << QThread::currentThreadId();
    toggleButton(connectResult == VncStatusCode::VNC_OK);

    if (connectResult == VncStatusCode::VNC_ERROR)
    {
        QMessageBox::about(this, "tips", "connection error");
    }
    if (connectResult == VncStatusCode::VNC_PASSWORD_ERROR)
    {
        QMessageBox::about(this, "tips", "password error");
    }
}

void MainWindow::pwdSlot()
{
    qDebug() << "MainWindow::pwdSlot" <<  QThread::currentThreadId();
    QString qpwd = QInputDialog::getText(this, "tips", "input password", QLineEdit::Password);
    ui->widget->setVncPassword(qpwd);
}

void MainWindow::vncErrorSlot(int err)
{
    qDebug("MainWindow::vncErrorSlot");
    if (err == VncStatusCode::VNC_ERROR)
    {
        QMessageBox::about(this, "tips", "server error");
    }
    else if (err == VncStatusCode::VNC_AUTH_NOT_SUPPORT)
    {
        QMessageBox::about(this, "tips", "unsupport auth type");
    }
}

void MainWindow::toggleButton(bool connected)
{
    if (connected)
    {
        ui->pushButton->setDisabled(true);
        ui->pushButton_2->setDisabled(false);
        ui->pushButton_3->setDisabled(false);
    }
    else
    {
        ui->pushButton->setDisabled(false);
        ui->pushButton_2->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
    }
}
