/***************************************************************************
**
** Copyright (C) 2012 Research In Motion
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtNfc module of the Qt Toolkit.
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

#ifndef QNXNFCEVENTFILTER_H
#define QNXNFCEVENTFILTER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QAbstractNativeEventFilter>
#include <QAbstractEventDispatcher>
//#include "qnxnfcmanager_p.h"
#include <bps/navigator.h>
#include "bps/bps.h"
#include "bps/navigator_invoke.h"
#include "../qnfcglobal.h"
#include "../qndefmessage.h"

QT_BEGIN_NAMESPACE

class QNXNFCEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    QNXNFCEventFilter();

    void installOnEventDispatcher(QAbstractEventDispatcher *dispatcher);
    void uninstallEventFilter();
private:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);

    QAbstractNativeEventFilter *prevFilter;

signals:
    void ndefEvent(const QNdefMessage &msg);
};

QT_END_NAMESPACE

#endif // QNXNFCEVENTFILTER_H
