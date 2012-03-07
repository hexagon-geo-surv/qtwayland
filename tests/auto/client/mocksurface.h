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

#include <qglobal.h>
#include <wayland-server.h>

#include "mockcompositor.h"

namespace Impl {

class Surface
{
public:
    Surface(wl_client *client, uint32_t id, Compositor *compositor);
    ~Surface();

    Compositor *compositor() const { return m_compositor; }
    wl_surface *handle() { return &m_surface; }

    QSharedPointer<MockSurface> mockSurface() const { return m_mockSurface; }

private:
    wl_surface m_surface;
    wl_buffer *m_buffer;

    Compositor *m_compositor;
    QSharedPointer<MockSurface> m_mockSurface;

    wl_list m_frameCallbackList;

    friend void surface_attach(wl_client *client, wl_resource *surface,
                               wl_resource *buffer, int x, int y);
    friend void surface_damage(wl_client *client, wl_resource *surface,
                               int32_t x, int32_t y, int32_t width, int32_t height);
    friend void surface_frame(wl_client *client, wl_resource *surface, uint32_t callback);
};

}
