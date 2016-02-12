/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
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
#include "browserapplication.h"
#include <QtPlugin>
#include <QtGui>

#ifdef Q_OS_WIN
#include "explorerstyle.h"
#endif

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(htmls);
    Q_INIT_RESOURCE(data);
//#ifdef Q_WS_X11
//  QApplication::setGraphicsSystem(QString::fromLatin1("raster"));
//#endif
    BrowserApplication application(argc, argv);
    if (!application.isRunning())
        return 0;
//#ifdef Q_OS_WIN
  //application.setStyle(new ExplorerStyle);
//#endif

    qApp->setStyleSheet(QLatin1String("\
                                      QLineEdit {\
                                      border: 1px solid lightgrey;\
                                      padding-left: 5px;\
                                      padding-right: 5px;\
                                      padding-top: 0px;\
                                      padding-bottom: 0px;\
                                      font-size: 13px;\
                                      border-radius: 2px;\
                                      border-top-left-radius: 10px;\
                                      border-top-right-radius: 10px;\
                                      border-bottom-left-radius: 10px;\
                                      border-bottom-right-radius: 10px;\
                                      min-width: 75px;\
                                      min-height: 23px;\
                                      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                  stop: 0 white,\
                                                                  stop: 1.0 rgb(230, 230, 230));\
                                      }\
                                      QLineEdit:hover {\
                                      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                  stop: 0 white,\
                                                                  stop: 1.0 rgb(240, 240, 240));\
                                      }\
                                      QLineEdit:pressed {\
                                      background:white;\
                                      }\
                                      QMenu {\
                                      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                  stop: 0 white,\
                                                                  stop: 1.0 rgb(230, 230, 230));\
                                      border: 1px solid lightgrey;\
                                      }\
                                      QMenu::item {\
                                      background-color: transparent;\
                                      }\
                                      QMenu::item:selected {\
                                      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                  stop: 0 white,\
                                                                  stop: 0.49 rgb(180, 180, 180),\
                                                                  stop: 1.0 rgb(170, 170, 170));\
                                      }\
                                      QMenu::separator {\
                                          height: 1px;\
                                          background: lightgrey;\
                                          margin-left: 5px;\
                                          margin-right: 10px;\
                                      }\
                                      QToolBar{\
                                      border: 1px solid lightgrey;\
                                      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                  stop: 0 white,\
                                                                  stop: 0.49 rgb(230, 230, 230),\
                                                                  stop: .5 rgb(220, 220, 220),\
                                                                  stop: 1.0 rgb(215, 215, 215));\
                                      }\
                                      "));

    //QApplication::setStyle(new QCleanlooksStyle);
    application.newMainWindow();
    return application.exec();
}
