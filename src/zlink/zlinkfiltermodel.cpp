/*
 * Copyright 2010 Daniel Graziotin <daniel.graziotin@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "history.h"

#include "autosaver.h"
#include "browserapplication.h"
#include "historymanager.h"
#include "treesortfilterproxymodel.h"

#include <qbuffer.h>
#include <qclipboard.h>
#include <qdesktopservices.h>
#include <qheaderview.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qsettings.h>
#include <qstyle.h>
#include <qtemporaryfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qmimedata.h>

#include <qwebhistoryinterface.h>
#include <qwebsettings.h>

#include <qdebug.h>
#include <zlinkfiltermodel.h>

zLinkFilterModel::zLinkFilterModel(QAbstractItemModel *sourceModel, QObject *parent)
    : QAbstractProxyModel(parent)
    , m_loaded(false)
{
    setSourceModel(sourceModel);
}

int zLinkFilterModel::historyLocation(const QString &url) const
{
    load();
    const QUrl qUrl(url);
    if(!m_historyHash.contains(qUrl.host()))
        return 0;

    return sourceModel()->rowCount() - m_historyHash.value(qUrl.toString());
}

QVariant zLinkFilterModel::data(const QModelIndex &index, int role) const
{
    if(role == FrecencyRole && index.isValid()) {
        return m_filteredRows[index.row()].frecency;
    }

    return QAbstractProxyModel::data(index, role);
}

void zLinkFilterModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    if(sourceModel()) {
        disconnect(sourceModel(), SIGNAL(modelReset()), this, SLOT(sourceReset()));
        disconnect(sourceModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                   this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
        disconnect(sourceModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                   this, SLOT(sourceRowsInserted(const QModelIndex &, int, int)));
        disconnect(sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
                   this, SLOT(sourceRowsRemoved(const QModelIndex &, int, int)));
    }

    QAbstractProxyModel::setSourceModel(newSourceModel);

    if(sourceModel()) {
        m_loaded = false;
        connect(sourceModel(), SIGNAL(modelReset()), this, SLOT(sourceReset()));
        connect(sourceModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                this, SLOT(sourceDataChanged(const QModelIndex &, const QModelIndex &)));
        connect(sourceModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                this, SLOT(sourceRowsInserted(const QModelIndex &, int, int)));
        connect(sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
                this, SLOT(sourceRowsRemoved(const QModelIndex &, int, int)));
    }
}

void zLinkFilterModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

QVariant zLinkFilterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

void zLinkFilterModel::recalculateFrecencies()
{
    sourceReset();
}

void zLinkFilterModel::sourceReset()
{
    m_loaded = false;
    reset();
}

int zLinkFilterModel::rowCount(const QModelIndex &parent) const
{
    load();
    if(parent.isValid())
        return 0;
    return m_historyHash.count();
}

int zLinkFilterModel::columnCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : 2;
}

QModelIndex zLinkFilterModel::mapToSource(const QModelIndex &proxyIndex) const
{
    load();
    int sourceRow = sourceModel()->rowCount() - proxyIndex.internalId();
    return sourceModel()->index(sourceRow, proxyIndex.column());
}

QModelIndex zLinkFilterModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    load();
    QString url = sourceIndex.data(HistoryModel::UrlStringRole).toString();
    const QUrl qUrl(url);
    if(!m_historyHash.contains(qUrl.host()))
        return QModelIndex();

    int sourceOffset = sourceModel()->rowCount() - sourceIndex.row();

    QList<HistoryData>::iterator pos = qBinaryFind(m_filteredRows.begin(),
                                       m_filteredRows.end(), HistoryData(sourceOffset, -1));

    if(pos == m_filteredRows.end())
        return QModelIndex();

    return createIndex(pos - m_filteredRows.begin(), sourceIndex.column(), sourceOffset);
}

QModelIndex zLinkFilterModel::index(int row, int column, const QModelIndex &parent) const
{
    load();
    if(row < 0 || row >= rowCount(parent)
            || column < 0 || column >= columnCount(parent))
        return QModelIndex();

    return createIndex(row, column, m_filteredRows[row].tailOffset);
}

QModelIndex zLinkFilterModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

void zLinkFilterModel::load() const
{
    if(m_loaded)
        return;
    m_filteredRows.clear();
    m_historyHash.clear();
    m_historyHash.reserve(sourceModel()->rowCount());
    m_scaleTime = QDateTime::currentDateTime();
    for(int i = 0; i < sourceModel()->rowCount(); ++i) {
        QModelIndex idx = sourceModel()->index(i, 0);
        QString url = idx.data(HistoryModel::UrlStringRole).toString();
        const QUrl qUrl(url);
        // we keep zBrowser's internal URLs away from here
        if(isValid(qUrl)) {
            if(!m_historyHash.contains(qUrl.host())) {
                int sourceOffset = sourceModel()->rowCount() - i;
                m_filteredRows.append(HistoryData(sourceOffset, frecencyScore(idx)));
                m_historyHash.insert(qUrl.host(), sourceOffset);
            } else {
                // we already know about this url: just increment its frecency score
                QList<HistoryData>::iterator pos = qBinaryFind(m_filteredRows.begin(),
                                                   m_filteredRows.end(), HistoryData(m_historyHash[qUrl.host()], -1));
                Q_ASSERT(pos != m_filteredRows.end());
                pos->frecency += frecencyScore(idx);
            }
        }
    }
    m_loaded = true;
}

void zLinkFilterModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(start == end && start == 0);
    Q_UNUSED(end);
    if(!m_loaded)
        return;
    QModelIndex idx = sourceModel()->index(start, 0, parent);
    QString url = idx.data(HistoryModel::UrlStringRole).toString();
    const QUrl qUrl(url);
    int currentFrecency = 0;
    if(m_historyHash.contains(qUrl.host())) {
        QList<HistoryData>::iterator pos = qBinaryFind(m_filteredRows.begin(),
                                           m_filteredRows.end(), HistoryData(m_historyHash[qUrl.host()], -1));
        Q_ASSERT(pos != m_filteredRows.end());
        int realRow = pos - m_filteredRows.begin();
        currentFrecency = pos->frecency;
        beginRemoveRows(QModelIndex(), realRow, realRow);
        m_filteredRows.erase(pos);
        m_historyHash.remove(qUrl.host());
        endRemoveRows();
    }
    beginInsertRows(QModelIndex(), 0, 0);
    m_filteredRows.insert(0, HistoryData(sourceModel()->rowCount(), frecencyScore(idx) + currentFrecency));
    m_historyHash.insert(qUrl.host(), sourceModel()->rowCount());
    endInsertRows();
}

void zLinkFilterModel::sourceRowsRemoved(const QModelIndex &, int start, int end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    sourceReset();
}

/*
    Removing a continuous block of rows will remove filtered rows too as this is
    the users intention.
*/
bool zLinkFilterModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row < 0 || count <= 0 || row + count > rowCount(parent) || parent.isValid())
        return false;
    int lastRow = row + count - 1;
    disconnect(sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
               this, SLOT(sourceRowsRemoved(const QModelIndex &, int, int)));
    beginRemoveRows(parent, row, lastRow);
    int oldCount = rowCount();
    int start = sourceModel()->rowCount() - m_filteredRows[row].tailOffset;
    int end = sourceModel()->rowCount() - m_filteredRows[lastRow].tailOffset;
    sourceModel()->removeRows(start, end - start + 1);
    endRemoveRows();
    connect(sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            this, SLOT(sourceRowsRemoved(const QModelIndex &, int, int)));
    m_loaded = false;
    if(oldCount - count != rowCount())
        reset();
    return true;
}

bool zLinkFilterModel::isValid(const QUrl url)
{
    QString urlString = url.toString();
    return !urlString.contains(QString::fromLatin1(" "))
           && !urlString.contains(QString::fromLatin1("qrc:"))
           && !urlString.contains(QString::fromLatin1("about:"))
           && url.isValid();
}

int zLinkFilterModel::frecencyScore(const QModelIndex &sourceIndex) const
{
    QDateTime loadTime = sourceModel()->data(sourceIndex, HistoryModel::DateTimeRole).toDateTime();
    int days = loadTime.daysTo(m_scaleTime);

    if(days <= 1) {
        return 100;
    } else if(days < 5) {  // within the last 4 days
        return 90;
    } else if(days < 15) {  // within the last two weeks
        return 70;
    } else if(days < 31) {  // within the last month
        return 50;
    } else if(days < 91) {  // within the last 3 months
        return 30;
    }

    return 10;
}


