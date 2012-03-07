/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MOCKCOMPOSITOR_H
#define MOCKCOMPOSITOR_H

#include <pthread.h>
#include <qglobal.h>
#include <wayland-server.h>

#include <QImage>
#include <QMutex>
#include <QRect>
#include <QSharedPointer>
#include <QVariant>
#include <QVector>
#include <QWaitCondition>

namespace Impl {

typedef void (**Implementation)(void);

class Surface;

class Compositor
{
public:
    Compositor();
    ~Compositor();

    int fileDescriptor() const { return m_fd; }
    void dispatchEvents(int timeout = 0);

    uint32_t time() { return ++m_time; }

    static void setOutputGeometry(void *compositor, const QList<QVariant> &parameters);

    QVector<Surface *> surfaces() const;

    void addSurface(Surface *surface);
    void removeSurface(Surface *surface);

    static void setKeyboardFocus(void *data, const QList<QVariant> &parameters);
    static void sendMousePress(void *data, const QList<QVariant> &parameters);
    static void sendMouseRelease(void *data, const QList<QVariant> &parameters);
    static void sendKeyPress(void *data, const QList<QVariant> &parameters);
    static void sendKeyRelease(void *data, const QList<QVariant> &parameters);

private:
    static void bindCompositor(wl_client *client, void *data, uint32_t version, uint32_t id);
    static void bindInput(wl_client *client, void *data, uint32_t version, uint32_t id);
    static void bindOutput(wl_client *client, void *data, uint32_t version, uint32_t id);
    static void bindShell(wl_client *client, void *data, uint32_t version, uint32_t id);

    static void destroyInputResource(wl_resource *resource);

    void initShm();

    void sendOutputGeometry(wl_resource *resource);
    void sendOutputMode(wl_resource *resource);

    QRect m_outputGeometry;

    wl_display *m_display;
    wl_event_loop *m_loop;
    wl_shm *m_shm;
    int m_fd;

    wl_list m_outputResources;
    uint32_t m_time;

    wl_input_device m_input;
    QVector<Surface *> m_surfaces;
};

void registerResource(wl_list *list, wl_resource *resource);

}

class MockSurface
{
public:
    Impl::Surface *handle() const { return m_surface; }

    QImage image;

private:
    MockSurface(Impl::Surface *surface);
    friend class Impl::Compositor;
    friend class Impl::Surface;

    Impl::Surface *m_surface;
};

Q_DECLARE_METATYPE(QSharedPointer<MockSurface>)

class MockCompositor
{
public:
    MockCompositor();
    ~MockCompositor();

    void applicationInitialized();

    int waylandFileDescriptor() const;
    void processWaylandEvents();

    void setOutputGeometry(const QRect &rect);
    void setKeyboardFocus(const QSharedPointer<MockSurface> &surface);
    void sendMousePress(const QSharedPointer<MockSurface> &surface, const QPoint &pos);
    void sendMouseRelease(const QSharedPointer<MockSurface> &surface);
    void sendKeyPress(const QSharedPointer<MockSurface> &surface, uint code);
    void sendKeyRelease(const QSharedPointer<MockSurface> &surface, uint code);

    QSharedPointer<MockSurface> surface();

    void lock();
    void unlock();

private:
    struct Command
    {
        typedef void (*Callback)(void *target, const QList<QVariant> &parameters);

        Callback callback;
        void *target;
        QList<QVariant> parameters;
    };

    static Command makeCommand(Command::Callback callback, void *target);

    void processCommand(const Command &command);
    void dispatchCommands();

    static void *run(void *data);

    bool m_alive;
    bool m_ready;
    pthread_t m_thread;
    QMutex m_mutex;
    QWaitCondition m_waitCondition;

    Impl::Compositor *m_compositor;

    QList<Command> m_commandQueue;
};

#endif
