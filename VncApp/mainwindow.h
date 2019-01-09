#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private slots:
    void connectButtonClick();
    void viewOnlyButtonClick();
    void disconnectButtonClick();
    void resizeSlot(int x, int y);
    void pwdSlot();
    void connectedSlot(int connectResult);
    void vncErrorSlot(int err);
private:
    void toggleButton(bool connected = false);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
