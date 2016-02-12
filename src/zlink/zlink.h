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

#ifndef ZLINK_H
#define ZLINK_H


#include <qdatetime.h>
#include <qhash.h>
#include <qobject.h>
#include <qsortfilterproxymodel.h>
#include <qtimer.h>
#include <qurl.h>
#include <historymanager.h>
#include <qwebhistoryinterface.h>

/**
 * A specialized HistoryEntry that also holds the frecency value and favicon.ico.
 * The operators <, > and = are overloaded to automatically handle
 * sorting and comparison based on the frecency
 */
class HistoryFrecencyEntry: public HistoryEntry
{
public:
    /**
     * Constructor of a HistoryFrecencyEntry object. Every field is mandatory.
     * @param url the URL of the Entry (a domain or a host)
     * @param datetime the datetime of the last visit - not used
     * @param title the title of the last visited page - not used
     * @param frecency the retrieved frecency of the entry
     * @param icon the favicon.ico of the entry in base64 format
     */
    HistoryFrecencyEntry(const QString &url,
                         const QDateTime &datetime = QDateTime(),
                         const QString &title = QString(),
                         const int frecency = -1,
                         const QString &icon = QString());

    inline bool operator <(const HistoryFrecencyEntry &other) const {
        return frecency < other.frecency;
    }

    inline bool operator >(const HistoryFrecencyEntry &other) const {
        return frecency > other.frecency;
    }

    inline bool operator ==(const HistoryFrecencyEntry &other) const {
        return frecency == other.frecency;
    }
    /**
     * Holds the frecency (recency + frequency) of the Entry
     */
    int frecency;
    /**
     * Holds the favicon.ico file of the Entry
     */
    QString icon;
};

/**
 * zLink is a new modality inspired by Google Chrome, Safari and Opera.
 * Based on frecency data, it uses zinkFilterModel to retrieve the
 * most visited sites (hosts or domains). The list is created in descending
 * order (first entry is the one with highest frecency).
 */
class zLink : public QObject
{

    Q_OBJECT

public:
    /**
     * Constructor for also setting the maximum number of entries
     * to be computed by zLink
     * @param numberEntries the maximum number of entries
     */
    zLink(int numberEntries = 0, QObject * parent = 0);
    /**
     * Given a number n of entries, it retrieves the most n visited hosts
     * and encapsulates them in a QList of HistoryFrecencyEntry objects.
     * The order is descending, the first element of the list has the
     * highest frecency.
     * @param numberEntries how many HistoryFrecencyEntry must be generated
     * @return a list of HistoryFrecencyEntry objects, order descending
     * @see HistoryFrecencyEntry
     */
    QList<HistoryFrecencyEntry> mostVisitedEntries();
    /**
     * Converts a list of HistoryFrecencyEntry objects into a QString
     * encoded in HTML representing the objects.
     * @param mostVisitedEntries a list of the most
     * @return a QString representation in HTML of the most visited entries
     */
    QString mostVisitedEntriesHTML();
    /**
     * Loads a HTML file and replaces the place marker %1 in the HTML file with
     * the supplied QString. It returns the substituted HTML file as a QByteArray
     * object.
     * @param mostVisitedEntriesHTML the most visited entries
     * @return the zLink HTML page with the most visited entries
     */
    QByteArray zlinkPage(QString mostVisitedEntriesHTML);
    /**
     * Convenience method that should be the only one public.
     * Given a number of entries, it returns zLink's HTML page with the
     * most visited entries.
     * @param numberEntries how many HistoryFrecencyEntry must be generated
     * @return the complete zLink HTML page, ready to be displayed
     */
    QByteArray render();

    /**
     * @return true if the given URL is valid for zLink mode
     * @see zLinkFilterModel::isValid()
     */
    static bool isValid(const QUrl& url);

    /**
     * Getter for m_maxNumberEntries
     */
    int maxNumberEntries();

    /**
     * Default number of most visited entries to be considered
     */
    static const int s_defaultMaxNumberEntries = 8;

public slots:
    /**
     * Convenience method that should be the only one public.
     * Given a number of entries, it returns zLink's HTML page with the
     * most visited entries.
     * @param numberEntries how many HistoryFrecencyEntry must be generated
     * @return the complete zLink HTML page, ready to be displayed
     */
    void calculate();

private:
    /**
     * Converts a QIcon objects to its base64 representation
     * @param icon a QIcon
     * @return its representation in base64
     */
    QString toBase64(QIcon& icon);

    /**
     * Comparison method to be used with qSort()
     * @param a the first entry
     * @param b the second entry
     * @return true if a > b (a most visited)
     */
    static bool compareHistoryFrecencyEntries(const HistoryFrecencyEntry& a,
            const HistoryFrecencyEntry& b);
    /**
     * QList of HistoryFrecencyEntry that holds the most visited
     * entries
     */
    QList<HistoryFrecencyEntry> m_mostVisitedEntries;
    /**
     * Interval, in milliseconds, of the timer responsible
     * to call the function to update zLink's list
     */
    static const int s_msUpdateInterval = 5000;
    /**
     * Holds the maximum number of entries to be retrieved by
     * zLink
     */
    int m_maxNumberEntries;
    /**
     * Timer resposible for calling calculate() each s_msUpdateInterval
     * milliseconds
     */
    QTimer* m_timer;

};

#endif // ZLINK_H
