/**
 * Copyright 2009 Christopher Eby <kreed@kreed.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Arora nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef EDITABLETOOLBAR_H
#define EDITABLETOOLBAR_H

#include <qbasictimer.h>
#include <qtoolbar.h>

class AnimatedSpacer;
template<class K, class V> class QHash;

class EditableToolBar : public QToolBar
{
    Q_OBJECT
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)

public:
    EditableToolBar(const QString &name, QWidget *parent = 0);
    EditableToolBar(QWidget *parent = 0);
    ~EditableToolBar();

    bool isEditable() const { return m_editable; }
    void setEditable(bool editable);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

    bool restoreState(const QHash<QString, QAction*> &actionMap, const QByteArray &state);
    QAction *nearestActionAt(const QPoint &pos) const;

    void setPossibleActions(const QList<QAction*> &actions);
    QList<QAction*> possibleActions() const;

    void setDefaultActions(const QStringList &actions);
    QStringList defaultActions() const;

protected:
    void actionEvent(QActionEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *);
    void timerEvent(QTimerEvent *);

    bool event(QEvent *);

private:
    QList<QAction*> m_possibleActions;
    QStringList m_defaultActions;
    bool eventDragMove(QDragMoveEvent *);
    bool eventMouseMove(QMouseEvent *);
    bool eventMousePress(QMouseEvent *);
    bool eventMouseRelease(QMouseEvent *);

    void removeCurrentSpacer();

    AnimatedSpacer *m_currentSpacer;
    QAction *m_currentSpacerLocation;
    QBasicTimer m_spacerTimer;
    bool m_editable;
    QWidget *m_resizing;
    int m_resizeFrom;
    int m_resizeMin;
    int m_resizeMax;
    int m_resizeDirection;
};

#endif
