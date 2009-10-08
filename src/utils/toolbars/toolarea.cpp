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

#include "toolarea.h"

#include "flowlayout.h"

#include <qevent.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qwidgetaction.h>

#ifdef Q_OS_MAC
#include <qapplication.h>
#endif

#define DRAG_MIMETYPE QLatin1String("application/x-arora-tool")

ToolArea::ToolArea(QWidget *parent)
    : QWidget(parent)
    , m_iconSize(24, 24)
{
    setAcceptDrops(true);
    setLayout(new FlowLayout(20, 20, 20));
}

void ToolArea::setIconSize(const QSize &size)
{
    if (size == m_iconSize)
        return;
    m_iconSize = size;
    foreach (QToolButton *button, findChildren<QToolButton*>())
        button->setIconSize(size);
}

void ToolArea::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    foreach (QToolButton *button, findChildren<QToolButton*>())
        button->setToolButtonStyle(style);
}

static QPixmap render(QWidget *widget, const QSize &maxSize)
{
    QSize size = widget->sizeHint();
    QPixmap pixmap(qMin(size.width(), maxSize.width()), qMin(size.height(), maxSize.height()));
    pixmap.fill(Qt::transparent);
    widget->render(&pixmap, QPoint(), QRegion(), QWidget::DrawChildren);
    return pixmap;
}

void ToolArea::addActions(const QList<QAction*> &actions)
{
    foreach (QAction *action, actions) {
        QWidget *widget;

        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction*>(action)) {
            widget = new QWidget;
            QLayout *boxLayout = new QVBoxLayout;

            QLabel *icon = new QLabel;
            icon->setPixmap(::render(widgetAction->defaultWidget(), size() / 2));
            icon->setAlignment(Qt::AlignCenter);
            icon->setProperty("ACTION", quintptr(action));
            boxLayout->addWidget(icon);

            QLabel *text = new QLabel;
            text->setText(action->text());
            text->setAlignment(Qt::AlignCenter);
            boxLayout->addWidget(text);

            widget->setLayout(boxLayout);
        } else {
            QToolButton *toolButton = new QToolButton;
            toolButton->setIconSize(m_iconSize);
            toolButton->setDefaultAction(action);
            // for some reason mac ignores this
            toolButton->setAutoRaise(true);
            // don't show the icon text
            toolButton->setText(action->text());
            toolButton->setEnabled(true);
            toolButton->setAttribute(Qt::WA_TransparentForMouseEvents);
            widget = toolButton;
        }

        layout()->addWidget(widget);
    }
}

void ToolArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QWidget *widget = 0;
        foreach (QObject *object, children()) {
            if (QWidget *child = qobject_cast<QWidget*>(object)) {
                if (child->geometry().contains(event->pos())) {
                    widget = child;
                    break;
                }
            }
        }
        if (!widget)
            return;

        QPixmap pixmap;
        QAction *action;
        if (QToolButton *button = qobject_cast<QToolButton*>(widget)) {
            action = button->defaultAction();
            pixmap = ::render(button, size() / 2);
        } else if (QLabel *label = qobject_cast<QLabel*>(widget->childAt(widget->mapFromParent(event->pos())))) {
            action = reinterpret_cast<QAction*>(label->property("ACTION").value<quintptr>());
            pixmap = *label->pixmap();
        } else {
            return;
        }

        QMimeData *mimeData = new QMimeData;
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << QSize(pixmap.width(), pixmap.height());
        out << quintptr(action);
        mimeData->setData(DRAG_MIMETYPE, data);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

#ifdef Q_OS_MAC
        // Mac crashes when dropping without this. (??)
        qApp->setOverrideCursor(cursor());
#endif
        drag->exec(Qt::MoveAction);
#ifdef Q_OS_MAC
        qApp->restoreOverrideCursor();
#endif
    } else {
        QWidget::mousePressEvent(event);
    }
}

void ToolArea::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(DRAG_MIMETYPE))
        event->acceptProposedAction();
    QWidget::dragEnterEvent(event);
}

void ToolArea::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(DRAG_MIMETYPE))
        event->acceptProposedAction();
    QWidget::dropEvent(event);
}
