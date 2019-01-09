#ifndef EEOVNC_H
#define EEOVNC_H
#if defined(EEOVNC_LIBRARY)
#  define EEOVNCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define EEOVNCSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QtCore>
#include <QImage>
#include <rfb/rfbclient.h>

class EEOVNCSHARED_EXPORT EeoVncClient : public QObject
{
    Q_OBJECT
public:
    explicit EeoVncClient(const QString& name, bool viewOnly = false) : QObject(NULL), m_name(name), m_viewOnly(viewOnly){}
    ~EeoVncClient();
    QImage& getScreen(){ return m_canvas; }
    QString& getPasswd(){ return m_passwd; }
    void setPasswd(QString pwd){ m_passwd = pwd; }
    bool getViewOnly(){ return m_viewOnly; }
    void setViewOnly(bool viewOnly){ m_viewOnly = viewOnly; }
    void stopVnc(){ m_stop = true; m_innerThread.waitForFinished(); }
    static void setStaticObj(EeoVncClient *thisObj){ m_vncObj = thisObj; }
    static EeoVncClient *getStaticObj(){ return m_vncObj; }

private:
    static rfbBool resizeNew(rfbClient *client);
    static char *getPwd(rfbClient *client);
    void runCycle();

signals:
    void connectedSignal(int connectedResult);
    void needPasswdSignal();

    void updateSignal();
    void resizeSignal(int x, int y);
    void vncErrorSignal(int errNo);

public slots:
    void doConnect(QString);
    void doKey(int key, int pressOrRelease);
    void doMouse(int x, int y, int pressOrRelease);

private:
    bool m_stop = false;
    bool m_viewOnly = false;
    QFuture<void> m_innerThread;
    QImage m_canvas;
    QString m_name;
    QString m_passwd;
    rfbClient *m_vncClient;
    static EeoVncClient *m_vncObj;
};
#endif // EEOVNC_H
