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

#include <qboxlayout.h>
#include <qhash.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qwidgetaction.h>

#include "browserapplication.h"
#include "edittoolbar.h"
#include "toolarea.h"

static EditToolBar *toolBar;
static QHash<QString, QAction*> actionMap;
static QByteArray state;

class StateCaller : public QObject
{
    Q_OBJECT

public slots:
    void saveState();
    void restoreState();
};

void StateCaller::saveState()
{
    state = toolBar->saveState();
}

void StateCaller::restoreState()
{
    toolBar->restoreState(actionMap, state);
}

static void createAction(const QString &text, QWidget *parent)
{
    QAction *action = new QAction(text, parent);
    action->setObjectName(text);
    actionMap.insert(action->objectName(), action);
    toolBar->addAction(action);
}

static void createWidgetAction(const QString &text, QWidget *parent)
{
    QWidgetAction *action = new QWidgetAction(parent);
    action->setDefaultWidget(new QLineEdit);
    action->setObjectName(text);
    actionMap.insert(action->objectName(), action);
    toolBar->addAction(action);
}

int main(int argc, char **argv)
{
    BrowserApplication application(argc, argv);

    QMainWindow window;
    StateCaller caller;

    toolBar = new EditToolBar("Toolbar");
    window.addToolBar(toolBar);

    createAction("Action 1", &window);
    createAction("Action 2", &window);
    createAction("Action 3", &window);

    toolBar->setEditable(true);

    createWidgetAction("LineEdit 1", &window);
    createWidgetAction("LineEdit 2", &window);

    QVBoxLayout *layout = new QVBoxLayout;
    ToolArea *area = new ToolArea;
    area->addActions(actionMap.values());
    layout->addWidget(area);
    QPushButton *button = new QPushButton(QLatin1String("Save state"));
    QObject::connect(button, SIGNAL(pressed()),
                     &caller, SLOT(saveState()));
    layout->addWidget(button);
    button = new QPushButton(QLatin1String("Restore state"));
    QObject::connect(button, SIGNAL(pressed()),
                     &caller, SLOT(restoreState()));
    layout->addWidget(button);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    window.setCentralWidget(widget);

    window.show();

    return application.exec();
}

#include "main_edittoolbar.moc"
