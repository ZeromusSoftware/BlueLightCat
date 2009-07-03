/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "searchlineedit.h"

#import <Cocoa/Cocoa.h>

#include <qmenu.h>
#include <qaction.h>

// http://th30z.netsons.org/2008/08/qt4-mac-searchbox-wrapper/

#ifdef QT_MAC_USE_COCOA

#import <qmaccocoaviewcontainer_mac.h>

class SearchWidget : public QMacCocoaViewContainer
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = 0);
    QSize sizeHint() const;
};

#else

#include <Carbon/Carbon.h>

// The SearchLineEdit class wraps a native HISearchField.
class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

    QSize sizeHint() const;

private:
    HIViewRef searchField;
    CFStringRef searchFieldText;

};

#endif


#ifdef Q_WS_MAC

#include <Carbon/Carbon.h>

static QMenu *createMenu(QWidget *parent);

#ifdef QT_MAC_USE_COCOA
SearchWidget::SearchWidget(QWidget *parent)
    : QMacCocoaViewContainer(0, parent)
{
    // Many Cocoa objects create temporary autorelease objects,
    // so create a pool to catch them.
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    // Create the NSSearchField, set it on the QCocoaViewContainer.
    NSSearchField *search = [[NSSearchField alloc] init];
    setCocoaView(search);

    // Use a Qt menu for the search field menu.
    QMenu *qtMenu = createMenu(this);
    NSMenu *nsMenu = qtMenu->macMenu(0);
    [[search cell] setSearchMenuTemplate:nsMenu];

    // Release our reference, since our super class takes ownership and we
    // don't need it anymore.
    [search release];

    // Clean up our pool as we no longer need it.
    [pool release];
}

QSize SearchWidget::sizeHint() const
{
    return QSize(150, 40);
}

#else

// The SearchWidget class wraps a native HISearchField.
SearchWidget::SearchWidget(QWidget *parent)
    :QWidget(parent)
{
    // Create a native search field and pass its window id to QWidget::create.
    searchFieldText = CFStringCreateWithCString(0, "search", 0);
    HISearchFieldCreate(NULL/*bounds*/, kHISearchFieldAttributesSearchIcon | kHISearchFieldAttributesCancel,
                        NULL/*menu ref*/, searchFieldText, &searchField);
    create(reinterpret_cast<WId>(searchField));

    // Use a Qt menu for the search field menu.
    QMenu *searchMenu = createMenu(this);
    MenuRef menuRef = searchMenu->macMenu(0);
    HISearchFieldSetSearchMenu(searchField, menuRef);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

SearchWidget::~SearchWidget()
{
    CFRelease(searchField);
    CFRelease(searchFieldText);
}

// Get the size hint from the search field.
QSize SearchWidget::sizeHint() const
{
    EventRef event;
    HIRect optimalBounds;
    CreateEvent(0, kEventClassControl,
        kEventControlGetOptimalBounds,
        GetCurrentEventTime(),
        kEventAttributeUserEvent, &event);

    SendEventToEventTargetWithOptions(event,
        HIObjectGetEventTarget(HIObjectRef(winId())),
        kEventTargetDontPropagate);

    GetEventParameter(event,
        kEventParamControlOptimalBounds, typeHIRect,
        0, sizeof(HIRect), 0, &optimalBounds);

    ReleaseEvent(event);
    return QSize(optimalBounds.size.width + 100, // make it a bit wider.
                 optimalBounds.size.height);
}

#endif

QMenu *createMenu(QWidget *parent)
{
    QMenu *searchMenu = new QMenu(parent);

    QAction * indexAction = searchMenu->addAction(QLatin1String("Index Search"));
    indexAction->setCheckable(true);
    indexAction->setChecked(true);

    QAction * fulltextAction = searchMenu->addAction(QLatin1String("Full Text Search"));
    fulltextAction->setCheckable(true);

    QActionGroup *searchActionGroup = new QActionGroup(parent);
    searchActionGroup->addAction(indexAction);
    searchActionGroup->addAction(fulltextAction);
    searchActionGroup->setExclusive(true);

    return searchMenu;
}

/*
ClearButton *SearchLineEdit::clearButton() const
{
    return 0;
}

SearchButton *SearchLineEdit::searchButton() const
{
    return 0;
}

void SearchLineEdit::init()
{
}
*/
SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QWidget(parent)
{
    nativeWidget = new SearchWidget(this);
    nativeWidget->move(2,2);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    nativeWidget->show();
}

QSize SearchLineEdit::sizeHint() const
{
    return nativeWidget->sizeHint() + QSize(6, 2);
}

QString SearchLineEdit::text() const
{
    return QString();
}

void SearchLineEdit::setText(const QString &text)
{
}

void SearchLineEdit::selectAll()
{
}

void SearchLineEdit::setInactiveText(const QString &text)
{
}

void SearchLineEdit::clear()
{
}



#endif // Q_WS_MAC

#include "searchlineedit_mac.moc"