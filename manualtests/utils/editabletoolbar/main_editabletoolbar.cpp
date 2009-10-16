/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#include <qapplication.h>
#include <qmainwindow.h>
#include <qsettings.h>
#include <qplaintextedit.h>
#include <qwidgetaction.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qdebug.h>

#include <editabletoolbar.h>
#include <editabletoolbardialog.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    virtual QMenu *createPopupMenu();

private slots:
    void customizeToolbar();

private:
    void createActions();
    EditableToolBar *toolBar;

    QAction *action1;
    QAction *action2;
    QAction *action3;
    QWidgetAction *actionWidget1;
};

MainWindow::MainWindow()
{
    createActions();

    toolBar = new EditableToolBar(tr("&Navigation"), this);
    toolBar->setObjectName("navigation");
    QList<QAction*> toolBarActions;
    toolBarActions.append(action1);
    toolBarActions.append(action2);
    toolBarActions.append(action3);
    toolBarActions.append(actionWidget1);
    toolBar->setPossibleActions(toolBarActions);
    QStringList defaultToolBarActions;
    defaultToolBarActions.append(action1->objectName());
    defaultToolBarActions.append(action2->objectName());
    defaultToolBarActions.append(actionWidget1->objectName());
    toolBar->setDefaultActions(defaultToolBarActions);
    addToolBar(toolBar);

    setCentralWidget(new QPlainTextEdit(this));
    resize(640, 480);

    QSettings settings;
    toolBar->restoreState(settings.value("toolbar").toByteArray());
    restoreState(settings.value("mainWindow").toByteArray());

    customizeToolbar();
}

void MainWindow::createActions()
{
    action1 = new QAction(tr("Back"), this);
    action1->setIcon(QIcon::fromTheme(QLatin1String("go-previous")));
    action1->setObjectName("action_1");

    action2 = new QAction(tr("Forward"), this);
    action2->setIcon(QIcon::fromTheme(QLatin1String("go-next")));
    action2->setObjectName("action_2");

    action3 = new QAction(tr("Reload"), this);
    action3->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    action3->setObjectName("action_3");

    actionWidget1 = new QWidgetAction(this);
    QLineEdit *lineEdit = new QLineEdit;
    lineEdit->resize(200, lineEdit->sizeHint().height());
    QPixmap pixmap(lineEdit->size());
    pixmap.fill(Qt::transparent);
    lineEdit->render(&pixmap, QPoint(), QRegion(), QWidget::DrawChildren);
    actionWidget1->setIcon(pixmap);
    actionWidget1->setDefaultWidget(lineEdit);
    actionWidget1->setObjectName("actionWidget_1");
    actionWidget1->setText("Location Bar");
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("toolbar", toolBar->saveState());
    settings.setValue("mainWindow", saveState());
}

QMenu *MainWindow::createPopupMenu()
{
    QMenu *menu = QMainWindow::createPopupMenu();
    menu->addAction(tr("Remove action..."));
    menu->addAction(tr("Customize Toolbar..."), this, SLOT(customizeToolbar()));
    return menu;
}

void MainWindow::customizeToolbar()
{
    ToolBarDialog *dialog = new ToolBarDialog(toolBar);
    dialog->show();
}


int main(int argc, char **argv)
{
    QApplication application(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}

#include "main_edittoolbar.moc"

