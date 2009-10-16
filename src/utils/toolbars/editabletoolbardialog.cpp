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

#include "editabletoolbardialog.h"

#include "editabletoolbar.h"

#include <qmainwindow.h>
#include <qinputdialog.h>
#include <qmimedata.h>
#include <qwidgetaction.h>

ActionModel::ActionModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant ActionModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row > rowCount())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return actions[row]->text();
        break;
    case Qt::DecorationRole: {
        QIcon icon = actions[row]->icon();
        if (icon.availableSizes().count() == 1)
            return icon.pixmap(icon.availableSizes().at(0));
        return icon;
        break;
    }
    default:
        break;
    }
    return QVariant();
}

int ActionModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : actions.count();
}

Qt::ItemFlags ActionModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    return defaultFlags;
}

QStringList ActionModel::mimeTypes() const
{
    QStringList types;
    types << "application/x-editable-toolbar";
    return types;
}

QMimeData *ActionModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.count() != 1)
        return 0;

    QMimeData *mimeData = new QMimeData();
    QModelIndex index = indexes.first();
    QString objectName = actions[index.row()]->objectName();
    mimeData->setData(mimeTypes().first(), objectName.toUtf8());
    return mimeData;
}

ToolBarDialog::ToolBarDialog(EditableToolBar *parent)
    : QDialog(parent)
    , toolBar(parent)
{
    setupUi(this);

    ActionModel *actionModel = new ActionModel(this);
    actionModel->actions = toolBar->possibleActions();
    listView->setModel(actionModel);
    listView->setDragEnabled(true);
    listView->setIconSize(QSize(24, 24));

    m_toolButtonModes->addItem(tr("Icons"));
    m_toolButtonModes->addItem(tr("Text"));
    m_toolButtonModes->addItem(tr("Text Beside Icons"));
    m_toolButtonModes->addItem(tr("Text Under Icons"));
#if QT_VERSION >= 0x040600
    m_toolButtonModes->addItem(tr("System Default"));
#endif

    m_toolButtonModes->setCurrentIndex(parent->toolButtonStyle());
    m_iconSizes->setValue(parent->iconSize().width());

    connect(m_toolButtonModes, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeToolButtonStyle(int)));
    connect(m_iconSizes, SIGNAL(valueChanged(int)),
            this, SLOT(changeIconSize(int)));

#if defined(Q_WS_MAC)
    m_newToolBarButton->hide();
#endif
    toolBar->setEditable(true);
}

ToolBarDialog::~ToolBarDialog()
{
    toolBar->setEditable(false);
}

void ToolBarDialog::changeToolButtonStyle(int i)
{
    toolBar->setToolButtonStyle(Qt::ToolButtonStyle((i)));
}

void ToolBarDialog::changeIconSize(int size)
{
    toolBar->setIconSize(QSize(size, size));
}

