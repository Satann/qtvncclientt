#include "EeoVncClient.h"
#include <QtConcurrent/QtConcurrent>
#include <QImage>
#include <rfb/rfbclient.h>


EeoVncClient *EeoVncClient::m_vncObj = NULL;

EeoVncClient::~EeoVncClient()
{
    if (m_vncClient)
    {
        rfbClientCleanup(m_vncClient);
        m_vncClient = NULL;
    }
}

rfbBool EeoVncClient::resizeNew(rfbClient *client)
{
    EeoVncClient::getStaticObj()->getScreen() = QImage(client->width, client->height, QImage::Format_RGB32);
    client->updateRect.x = client->updateRect.y = 0;
    client->updateRect.w = client->width;
    client->updateRect.h = client->height;
    client->frameBuffer = EeoVncClient::getStaticObj()->getScreen().bits();
    client->format.redShift = 16;
    client->format.blueShift = 0;
    emit EeoVncClient::getStaticObj()->resizeSignal(client->width, client->height);
    return TRUE;
}

char *EeoVncClient::getPwd(rfbClient *client)
{
    emit EeoVncClient::getStaticObj()->needPasswdSignal();
    char* password = (char*)malloc(24);
    memset(password, 0, 24);
    std::string tmpString = EeoVncClient::getStaticObj()->getPasswd().toStdString();
    std::strcpy(password, tmpString.c_str());
    return password;
}

void EeoVncClient::doConnect(QString host)
{
    m_vncClient = rfbGetClient(8, 3, 4);
    m_vncClient->canHandleNewFBSize = TRUE;
    m_vncClient->MallocFrameBuffer = &EeoVncClient::resizeNew;
    m_vncClient->GetPassword = &EeoVncClient::getPwd;
    m_vncClient->listenPort = LISTEN_PORT_OFFSET;
    m_vncClient->listen6Port = LISTEN_PORT_OFFSET;

    int argc = 2;
    const char* argv[2];
    QString appName = qApp->applicationFilePath();
    if (!m_name.isEmpty())
    {
        appName = m_name;
    }
    std::string app(appName.toStdString());
    std::string host1 = host.toStdString();
    argv[0] = app.c_str();
    argv[1] = host1.c_str();
    int rfbRet = rfbInitClient(m_vncClient, &argc, (char**)argv);
    if (rfbRet != rfb_ok)
    {
        qDebug() << "rfbRet error" << QThread::currentThreadId();
        m_vncClient = NULL;
    }
    emit connectedSignal(rfbRet);
    if (rfbRet == rfb_ok)
    {
        m_innerThread = QtConcurrent::run(this, &EeoVncClient::runCycle);
    }
    else
    {
        qDebug() << "client connect error";
    }
    return;
}

void EeoVncClient::doKey(int k, int b)
{
    if (m_vncClient != NULL && !m_viewOnly)
    {
        SendKeyEvent(m_vncClient, k, b);
    }
}

void EeoVncClient::doMouse(int x, int y, int msk)
{
    if (m_vncClient != NULL && !m_viewOnly)
    {
        SendPointerEvent(m_vncClient, x, y, msk);
    }
}

void EeoVncClient::runCycle()
{
    while (!m_stop && m_vncClient)
    {
        int err = 0;
        err = WaitForMessage(m_vncClient, 1000);

        if (err < 0)
        {
            if (m_vncClient)
            {
                rfbClientCleanup(m_vncClient);
                m_vncClient = NULL;
                emit vncErrorSignal(rfb_error);
            }
            break;
        }
        bool bHandle = HandleRFBServerMessage(m_vncClient);

        if (!bHandle)
        {
            if (m_vncClient)
            {
                rfbClientCleanup(m_vncClient);
                m_vncClient = NULL;
                emit vncErrorSignal(rfb_error);
            }
            break;
        }
        emit updateSignal();
    }
    qDebug("runcycle end");
}
