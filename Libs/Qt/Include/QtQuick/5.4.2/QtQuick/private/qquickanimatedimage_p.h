/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQuick module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKANIMATEDIMAGE_P_H
#define QQUICKANIMATEDIMAGE_P_H

#include "qquickimage_p.h"

#ifndef QT_NO_MOVIE

QT_BEGIN_NAMESPACE

class QMovie;
class QQuickAnimatedImagePrivate;

class Q_AUTOTEST_EXPORT QQuickAnimatedImage : public QQuickImage
{
    Q_OBJECT

    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(int currentFrame READ currentFrame WRITE setCurrentFrame NOTIFY frameChanged)
    Q_PROPERTY(int frameCount READ frameCount)

    // read-only for AnimatedImage
    Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)

public:
    QQuickAnimatedImage(QQuickItem *parent=0);
    ~QQuickAnimatedImage();

    bool isPlaying() const;
    void setPlaying(bool play);

    bool isPaused() const;
    void setPaused(bool pause);

    int currentFrame() const;
    void setCurrentFrame(int frame);

    int frameCount() const;

    // Extends QQuickImage's src property
    virtual void setSource(const QUrl&);
    virtual QSize sourceSize();

Q_SIGNALS:
    void playingChanged();
    void pausedChanged();
    void frameChanged();
    void sourceSizeChanged();

private Q_SLOTS:
    void movieUpdate();
    void movieRequestFinished();
    void playingStatusChanged();

protected:
    virtual void load();
    void componentComplete();

private:
    Q_DISABLE_COPY(QQuickAnimatedImage)
    Q_DECLARE_PRIVATE(QQuickAnimatedImage)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickAnimatedImage)

#endif // QT_NO_MOVIE

#endif // QQUICKANIMATEDIMAGE_P_H
