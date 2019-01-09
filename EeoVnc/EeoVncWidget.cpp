#include "EeoVncWidget.h"
#include "EeoVncClient.h"
#include <QtWidgets>

struct { int sdl; int rfb; }
g_buttonMapping[] =
{
    { 1, rfbButton1Mask },
    { 2, rfbButton2Mask },
    { 3, rfbButton3Mask },
    { 4, rfbButton4Mask },
    { 5, rfbButton5Mask },
    { 0, 0 }
};

EeoVncWidget::EeoVncWidget(QWidget *parent) : QWidget(parent)
{
}

static rfbKeySym SDL_key2rfbKeySym(QKeyEvent *e)
{
    rfbKeySym k = 0;
    int sym = e->key();
    switch (sym)
    {
    case Qt::Key_Backspace: k = XK_BackSpace; break;
    case Qt::Key_Tab: k = XK_Tab; break;
    case Qt::Key_Clear: k = XK_Clear; break;
    case Qt::Key_Return: k = XK_Return; break;
    case Qt::Key_Pause: k = XK_Pause; break;
    case Qt::Key_Escape: k = XK_Escape; break;
    case Qt::Key_Delete: k = XK_Delete; break;
    case Qt::Key_0: k = XK_KP_0; break;
    case Qt::Key_1: k = XK_KP_1; break;
    case Qt::Key_2: k = XK_KP_2; break;
    case Qt::Key_3: k = XK_KP_3; break;
    case Qt::Key_4: k = XK_KP_4; break;
    case Qt::Key_5: k = XK_KP_5; break;
    case Qt::Key_6: k = XK_KP_6; break;
    case Qt::Key_7: k = XK_KP_7; break;
    case Qt::Key_8: k = XK_KP_8; break;
    case Qt::Key_9: k = XK_KP_9; break;
    case Qt::Key_Period: k = XK_KP_Decimal; break;
    case Qt::Key_Slash: k = XK_KP_Divide; break;
    case Qt::Key_multiply: k = XK_KP_Multiply; break;
    case Qt::Key_Minus: k = XK_KP_Subtract; break;
    case Qt::Key_Plus: k = XK_KP_Add; break;
    case Qt::Key_Enter: k = XK_KP_Enter; break;
    case Qt::Key_Equal: k = XK_KP_Equal; break;
    case Qt::Key_Up: k = XK_Up; break;
    case Qt::Key_Down: k = XK_Down; break;
    case Qt::Key_Right: k = XK_Right; break;
    case Qt::Key_Left: k = XK_Left; break;
    case Qt::Key_Insert: k = XK_Insert; break;
    case Qt::Key_Home: k = XK_Home; break;
    case Qt::Key_End: k = XK_End; break;
    case Qt::Key_PageUp: k = XK_Page_Up; break;
    case Qt::Key_PageDown: k = XK_Page_Down; break;
    case Qt::Key_F1: k = XK_F1; break;
    case Qt::Key_F2: k = XK_F2; break;
    case Qt::Key_F3: k = XK_F3; break;
    case Qt::Key_F4: k = XK_F4; break;
    case Qt::Key_F5: k = XK_F5; break;
    case Qt::Key_F6: k = XK_F6; break;
    case Qt::Key_F7: k = XK_F7; break;
    case Qt::Key_F8: k = XK_F8; break;
    case Qt::Key_F9: k = XK_F9; break;
    case Qt::Key_F10: k = XK_F10; break;
    case Qt::Key_F11: k = XK_F11; break;
    case Qt::Key_F12: k = XK_F12; break;
    case Qt::Key_F13: k = XK_F13; break;
    case Qt::Key_F14: k = XK_F14; break;
    case Qt::Key_F15: k = XK_F15; break;
    case Qt::Key_NumLock: k = XK_Num_Lock; break;
    case Qt::Key_CapsLock: k = XK_Caps_Lock; break;
    case Qt::Key_ScrollLock: k = XK_Scroll_Lock; break;
    case Qt::Key_Shift: k = XK_Shift_L; break;
    case Qt::Key_Control: k = XK_Control_L; break;
    case Qt::Key_Alt: k = XK_Alt_L; break;
    case Qt::Key_Super_L: k = XK_Super_L; break;
    case Qt::Key_Super_R: k = XK_Super_R; break;
    case Qt::Key_Mode_switch: k = XK_Mode_switch; break;
    case Qt::Key_Help: k = XK_Help; break;
    case Qt::Key_Print: k = XK_Print; break;
    case Qt::Key_SysReq: k = XK_Sys_Req; break;
    default: break;
    }

    if (k == 0 && Qt::ControlModifier & e->modifiers())
    {
        k = sym + 32;
    }
    if (k == 0 && (Qt::ShiftModifier & e->modifiers()))
    {
        k = sym;
    }
    if (k == 0)
    {
        QString qText = e->text();
        std::string stdString = qText.toStdString();
        const char *ascCode = stdString.c_str();
        k = *ascCode;
    }
    return k;
}

void EeoVncWidget::bindClient(const EeoVncClient *vncObj)
{
    connect(vncObj, SIGNAL(needPasswdSignal()), this, SIGNAL(onPwd()), Qt::BlockingQueuedConnection);
    connect(vncObj, SIGNAL(connectedSignal(int)), this, SLOT(connectedSlot(int)));
    connect(vncObj, SIGNAL(resizeSignal(int, int)), this, SLOT(resizeSlot(int, int)));
    connect(vncObj, SIGNAL(updateSignal()), this, SLOT(updateSlot()));
    connect(vncObj, SIGNAL(vncErrorSignal(int)), this, SLOT(vncErrorSlot(int)));

    connect(this, SIGNAL(queryConnect(QString)), vncObj, SLOT(doConnect(QString)));
    connect(this, SIGNAL(queryKey(int, int)), vncObj, SLOT(doKey(int, int)));
    connect(this, SIGNAL(queryPointer(int, int, int)), vncObj, SLOT(doMouse(int, int, int)));
}

void EeoVncWidget::initVnc()
{
    m_vncObj = new EeoVncClient("name");
    m_vncObj->setStaticObj(m_vncObj);
    bindClient(m_vncObj);
    connect(&m_workThread, SIGNAL(finished()), m_vncObj, SLOT(deleteLater()));
    m_vncObj->moveToThread(&m_workThread);
    m_workThread.start();
}

void EeoVncWidget::closeVnc()
{
    if (m_vncObj)
    {
        m_vncObj->stopVnc();
        m_vncObj = NULL;
    }
    m_workThread.exit(0);
    m_workThread.wait();
    m_isConnect = false;
}

void EeoVncWidget::connectVnc(const QString &host)
{
    qDebug("EeoVncWidget::buttonPressed:%d", QThread::currentThreadId());
    if (m_vncObj == NULL)
    {
        initVnc();
    }
    emit queryConnect(host);
}

void EeoVncWidget::setVncPassword(const QString& pwd)
{
    m_vncObj->setPasswd(pwd);
}

void EeoVncWidget::setVncViewOnly(bool viewOnly)
{
    m_vncObj->setViewOnly(viewOnly);
}

bool EeoVncWidget::getVncViewOnly()
{
    return m_vncObj->getViewOnly();
}

void EeoVncWidget::resizeSlot(int x, int y)
{
    emit onResize(x, y);
    resize(x, y);
}

void EeoVncWidget::updateSlot()
{
    update();
}

void EeoVncWidget::connectedSlot(int connectResult)
{
    if (connectResult == rfb_ok)
    {
        m_isConnect = true;
    }
    emit onConnected(connectResult);
}


void EeoVncWidget::vncErrorSlot(int err)
{
    m_isConnect = false;
    if (err == rfb_error || err == net_error)
    {
        err = VNC_ERROR;
    }
    emit onError(err);
}

bool EeoVncWidget::event(QEvent *e)
{
    if (m_isConnect)
    {
        if (e->type() == QEvent::HoverMove || e->type() == QEvent::MouseMove)
        {
            QHoverEvent *hoverEvent = (QHoverEvent*)e;
            m_buttonMask &= ~(rfbButton4Mask | rfbButton5Mask);
            emit queryPointer(hoverEvent->pos().x(), hoverEvent->pos().y(), m_buttonMask);
            return true;
        }
        else if (e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease)
        {
            QKeyEvent *kEvent = (QKeyEvent*)e;
            emit queryKey(SDL_key2rfbKeySym(kEvent), e->type() == QEvent::KeyPress ? TRUE : FALSE);
            return true;
        }
        else if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease || e->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent *mouseEvent = (QMouseEvent*)e;
            int state = mouseEvent->button();
            if (state == Qt::MouseButton::RightButton)
            {
                state++;
            }
            for (int i = 0; g_buttonMapping[i].sdl; i++)
            {
                if (state == g_buttonMapping[i].sdl)
                {
                    state = g_buttonMapping[i].rfb;
                    if (e->type() == QMouseEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick)
                    {
                        m_buttonMask |= state;
                    }
                    if (mouseEvent->type() == QMouseEvent::MouseButtonRelease)
                    {
                        m_buttonMask &= ~state;
                    }
                    break;
                }
            }
            emit queryPointer(mouseEvent->pos().x(), mouseEvent->pos().y(), m_buttonMask);
            m_buttonMask &= ~(rfbButton4Mask | rfbButton5Mask);
            return true;
        }
        else if (e->type() == QEvent::Wheel)
        {
            int steps = 0;
            QWheelEvent *wheelEvent = (QWheelEvent*)e;

            int py = wheelEvent->delta();
            if (py > 0)
            {
                for (; steps < py; steps += 60)
                {
                    emit queryPointer(wheelEvent->pos().x(), wheelEvent->pos().y(), rfbButton4Mask);
                    emit queryPointer(wheelEvent->pos().x(), wheelEvent->pos().y(), 0);
                }
            }
            else if (py < 0)
            {
                for (; steps > py; steps -= 60)
                {
                    emit queryPointer(wheelEvent->pos().x(), wheelEvent->pos().y(), rfbButton5Mask);
                    emit queryPointer(wheelEvent->pos().x(), wheelEvent->pos().y(), 0);
                }
            }
            return true;
        }
    }
    return QWidget::event(e);
}

void EeoVncWidget::paintEvent(QPaintEvent *paintEvent)
{
    if (m_vncObj && m_isConnect)
    {
        QPainter painter(this);
        painter.drawImage(QRect(0, 0, m_vncObj->getScreen().width(),
            m_vncObj->getScreen().height()), m_vncObj->getScreen());
    }
}
