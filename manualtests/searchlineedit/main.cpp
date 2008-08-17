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

#include <QtGui/QtGui>
#include "searchlineedit.h"

class RoundCorner : public QWidget
{
    Q_OBJECT
public:
    RoundCorner(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
};

RoundCorner::RoundCorner(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(QSize(10, 20));
}

void RoundCorner::paintEvent(QPaintEvent *event)
{
    bool leftSide = x() == 0;

    QPainterPath pathW;
    QRect r = rect();
    if (leftSide) {
        pathW.moveTo(width(), 0);
        r.adjust(0, 0, width(), 0);
        pathW.arcTo(r, 90, 180);
        pathW.lineTo(0, height());
        pathW.lineTo(0, 0);
    } else {
        pathW.moveTo(0, 0);
        r.adjust(- width(), 0, 0, 0);
        pathW.arcTo(r, 90, -180);
        pathW.lineTo(width(), height());
        pathW.lineTo(width(), 0);
    }

    QPainterPath path;
    r = rect();
    if (leftSide) {
        path.moveTo(width(), 0);
        r.adjust(0, 0, width(), 0);
        path.arcTo(r, 90, 180);
    } else {
        path.moveTo(0, 0);
        r.adjust(- width(), 0, 0, 0);
        path.arcTo(r, 90, -180);
    }


    QPainter painter(this);


    painter.setRenderHints(QPainter::Antialiasing);

    QLinearGradient linearGradient(QPoint(0, 0), QPoint(0, height()));
    linearGradient.setColorAt(0, QColor(104, 104, 104));
    linearGradient.setColorAt(1, QColor(154, 154, 154));
    QBrush brush(linearGradient);
    QPen pen(brush, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);
    painter.setBrush(linearGradient);
    painter.drawPath(pathW);

    QLinearGradient background(QPoint(0, 0), QPoint(0, height()));
    background.setColorAt(0, QColor(176, 176, 176));
    background.setColorAt(1, QColor(154, 154, 154));
    painter.setBrush(background);
    painter.drawPath(pathW);
}

#include "main.moc"

int main(int argc, char **argv)
{
    QApplication application(argc, argv);
    QMainWindow w;

{
    QWidget *window1 = new QWidget;
    RoundCorner *left = new RoundCorner(window1);
    RoundCorner *right = new RoundCorner(window1);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(left);
    layout->addWidget(right);
    window1->setLayout(layout);
    window1->resize(24, 24);
    window1->show();
}

    LineEdit *lineEdit = new LineEdit(&w);
    lineEdit->addWidget(new RoundCorner(lineEdit), LineEdit::LeftSide);
    lineEdit->addWidget(new RoundCorner(lineEdit), LineEdit::RightSide);
    w.addToolBar("foo")->addWidget(lineEdit);

    QWidget *window = new QWidget;
    QPushButton *button1 = new QPushButton("One");
    QPushButton *button2 = new QPushButton("Two");
    SearchLineEdit *s1 = new SearchLineEdit(window);
    SearchLineEdit *s2 = new SearchLineEdit(window);
    s2->menu()->addAction("f");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(s1);
    layout->addWidget(s2);
    layout->addWidget(button1);
    layout->addWidget(button2);

    window->setLayout(layout);
    w.setUnifiedTitleAndToolBarOnMac(true);
    w.show();
    w.setCentralWidget(window);
    return application.exec();
}

