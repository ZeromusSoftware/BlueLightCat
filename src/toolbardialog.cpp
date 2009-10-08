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

#include "toolbardialog.h"

#include "browsermainwindow.h"

#include <qinputdialog.h>

ToolBarDialog::ToolBarDialog(BrowserMainWindow *parent)
    : QDialog(parent)
{
    setupUi(this);

    m_toolArea->setIconSize(parent->iconSize());

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
    connect(m_newToolBarButton, SIGNAL(clicked()),
            this, SLOT(addToolBar()));
    connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(dialogButtonClicked(QAbstractButton*)));
    connect(this, SIGNAL(iconSizeChanged(QSize)),
            m_toolArea, SLOT(setIconSize(QSize)));
    connect(this, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
            m_toolArea, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));

#if defined(Q_WS_MAC)
    m_newToolBarButton->hide();
#endif
}

void ToolBarDialog::changeToolButtonStyle(int i)
{
    emit toolButtonStyleChanged(Qt::ToolButtonStyle(i));
}

void ToolBarDialog::changeIconSize(int size)
{
    emit iconSizeChanged(QSize(size, size));
}

void ToolBarDialog::addActions(const QList<QAction*> &actions)
{
    m_toolArea->addActions(actions);
}

void ToolBarDialog::addToolBar()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add ToolBar"),
            tr("Enter a name for the new toolbar:"), QLineEdit::Normal,
            QString(), &ok);
    if (ok && !text.isEmpty())
        emit newToolBarRequested(text);
}

void ToolBarDialog::dialogButtonClicked(QAbstractButton *button)
{
    switch (m_buttonBox->buttonRole(button)) {
    case QDialogButtonBox::ResetRole: {
        emit restoreDefaultToolBars();
        BrowserMainWindow *parent = qobject_cast<BrowserMainWindow*>(ToolBarDialog::parent());
        m_toolButtonModes->setCurrentIndex(parent->toolButtonStyle());
        m_iconSizes->setValue(parent->iconSize().width());
        break;
    }
    case QDialogButtonBox::AcceptRole:
        accept();
        break;
    case QDialogButtonBox::RejectRole:
        reject();
        break;
    default:
        break;
    }
}
