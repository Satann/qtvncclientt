#ifndef QTVNCWIDGET_H
#define QTVNCWIDGET_H
#if defined(EEOVNC_LIBRARY)
#  define EEOVNCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define EEOVNCSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QThread>
#include <QWidget>
enum VncStatusCode
{
    VNC_OK = 1,
    VNC_ERROR = 2,
    VNC_PASSWORD_ERROR = 3,
    VNC_AUTH_NOT_SUPPORT = 4
};


class EeoVncClient;
class EEOVNCSHARED_EXPORT EeoVncWidget : public QWidget
{
    Q_OBJECT
public:
    EeoVncWidget(QWidget *parent = Q_NULLPTR);
    void closeVnc();
    void connectVnc(const QString &host);
    bool isConnected(){ return m_isConnect; }
    void setVncPassword(const QString& pwd);
    void setVncViewOnly(bool viewOnly);
    bool getVncViewOnly();

protected:
    bool event(QEvent *);
    void paintEvent(QPaintEvent*);

private:
    void bindClient(const EeoVncClient*);
    void initVnc();

signals:
    void queryConnect(QString);
    void queryKey(int key, int pressOrRelease);
    void queryPointer(int x, int y, int pressOrRelease);
    
    //signals post to uper widget
    void onResize(int x, int y);
    void onConnected(int connectResult);
    void onPwd();
    void onError(int errNo);

private slots:
    void resizeSlot(int x, int y);
    void updateSlot();
    void connectedSlot(int connectedResult);
    void vncErrorSlot(int errNo);

private:
    int m_buttonMask = 0;
    bool m_isConnect = 0;
    QThread m_workThread;
    EeoVncClient* m_vncObj = NULL;
};

#endif // QTVNCWIDGET_H
