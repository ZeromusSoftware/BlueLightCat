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

#ifndef ZLINKFILTERMODEL_H
#define ZLINKFILTERMODEL_H

#include "modelmenu.h"

#include <qdatetime.h>
#include <qhash.h>
#include <qobject.h>
#include <qsortfilterproxymodel.h>
#include <qtimer.h>
#include <qurl.h>

#include <qwebhistoryinterface.h>
#include <history/historymanager.h>
#include <history/history.h>

/**
 *   Proxy model that removes any duplicate entries and saves the host of
 *   every URL passed. It stores in m_historyHash the hosts visited by the user
 *   of zBrowser and computes their frecencies.
 *   It is a modified version of HistoryFilterModel, but not its subclass yet.
 *   It may be in future.
 */
class zLinkFilterModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    /**
     * Constructor for settings the source model of the Proxy
     * @param sourceModel the data set on which the proxy operates. This is an
     *                    HistoryModel instance in zBrower
     * @param parent QT typical parent parameter
     */
    zLinkFilterModel(QAbstractItemModel *sourceModel, QObject *parent = 0);

    /**
     * Checks for the presence of a entry in the Hash table
     * @param url the URL to be checked. It is automatically converted to the form
     *         http://xxx.yyy.zz/
     * @return true if the host is contained in the hash table
     */
    inline bool historyContains(const QString &url) const {
        QUrl newUrl(url);
        return m_historyHash.contains(newUrl.host());
    }

    /**
     * @return the History position of the given URL
     */
    int historyLocation(const QString &url) const;

    /**
     * Roles for returning QVariant data
     */
    enum Roles {
        FrecencyRole = HistoryModel::MaxRole + 1,
        MaxRole = FrecencyRole
    };

    /**
     * Returns the model index in the proxy model that corresponds to the sourceIndex from the source model.
     */
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    /**
     * Returns the model index in the source model that corresponds to the proxyIndex in the proxy model.
     */
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    /**
     * Sets the sourceModel. In zBrowser, this is a HistoryModel instance
     */
    void setSourceModel(QAbstractItemModel *sourceModel);
    /**
     * @return the data for the given role and section in the header with the specified orientation.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    /**
     * @return the number of rows of the internal Hash Table
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /**
     * @return always 0 or 2 if there are problems
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    /**
     * @return The index of the item in the model specified by the given row, column and parent index.
     */
    QModelIndex index(int, int, const QModelIndex &parent = QModelIndex()) const;
    /**
     * @return the parent index of the given one
     */
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    /**
     * Removes count rows starting with the given row under parent parent from the model
     */
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    /**
     * @return the data stored under the given role for the item referred to by the index.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /**
     * Helper method to check the validity of a url
     * @return true if the url is valid for our model
     */
    static bool isValid(const QUrl url);
    /**
     * Forces to check the frecencies of the entries
     */
    void recalculateFrecencies();

private slots:
    /**
     * Signals
     */
    void sourceReset();
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void sourceRowsInserted(const QModelIndex &parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex &, int, int);

private:
    /**
     * Crucial method: it maps the history, converts every entry to their
     * hosts and populates the internal data structures
     */
    void load() const;

    /**
     * Helper data structure to hold the entries with their frecency
     */
    struct HistoryData {
        int tailOffset;
        int frecency;

        HistoryData(int off, int f = 0) : tailOffset(off), frecency(f) { }

        bool operator==(const HistoryData &other) const {
            return (tailOffset == other.tailOffset)
                   && (frecency == -1 || other.frecency == -1 || frecency == other.frecency);
        }
        bool operator!=(const HistoryData &other) const {
            return !(*this == other);
        }
        // like the actual history entries, our index mapping data is sorted in reverse
        bool operator<(const HistoryData &other) const {
            return (tailOffset > other.tailOffset);
        }
    };
    /**
     * Computes the frecency of the entry pointed by sourceIndex
     * @return the frecency score
     */
    int frecencyScore(const QModelIndex &sourceIndex) const;
    /**
     * List of entries of history with their frecency value and pointer
     */
    mutable QList<HistoryData> m_filteredRows;
    /**
     * Hash table holding the entry host and its frecency value
     */
    mutable QHash<QString, int> m_historyHash;
    /**
     * Holds whether the history has been loaded or not
     */
    mutable bool m_loaded;
    /**
     * Holds the current date time to be used as reference for calculating
     * the frecencies
     */
    mutable QDateTime m_scaleTime;
};


#endif // ZLINKFILTERMODEL_H

