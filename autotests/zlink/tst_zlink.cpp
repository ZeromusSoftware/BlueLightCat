#include "tst_zlink.h"
#include <QSignalSpy>

// This will be called before the first test function is executed.
// It is only called once. We save the browser history
void tst_zlink::initTestCase()
{
    m_manager = BrowserApplication::historyManager();
    m_history = m_manager->history();
    m_model = BrowserApplication::historyManager()->zLinkFilterModel();
    m_zlink = BrowserApplication::zlink();
    connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(testMinorFilterMethods()));
}


// This will be called after the last test function is executed.
// It is only called once.
void tst_zlink::cleanupTestCase()
{
}

// This will be called before each test function is executed. We clean browser history
void tst_zlink::init()
{
    QList<HistoryEntry> emtpyHistory;
    m_manager->history().clear();
    m_manager->setHistory(emtpyHistory);
    m_zlink->calculate();
}

// This will be called after every test function.
void tst_zlink::cleanup()
{
}

void tst_zlink::maxNumberEntriesEmptyHistory()
{
    int defaultMaxNumberEntries = zlink::s_defaultMaxNumberEntries;
    m_zlink = BrowserApplication::zlink(defaultMaxNumberEntries);

    QList<HistoryFrecencyEntry> last;

    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    m_zlink = BrowserApplication::zlink(defaultMaxNumberEntries + 1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    m_zlink = BrowserApplication::zlink(defaultMaxNumberEntries - 1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    m_zlink = BrowserApplication::zlink(0);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    m_zlink = BrowserApplication::zlink(-1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 0);
}

void tst_zlink::maxNumberEntriesNonEmptyHistory()
{
    int defaultMaxNumberEntries = zlink::s_defaultMaxNumberEntries;
    QList<HistoryFrecencyEntry> last;


    // m_zlink must contain http://twitter.com and http://facebook.com
    // that are two entries.
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_manager->addHistoryEntry("http://facebook.com/asd");
    m_manager->addHistoryEntry("http://facebook.com/poi");
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_manager->addHistoryEntry("http://twitter.com/oki");

    m_zlink = BrowserApplication::zlink(defaultMaxNumberEntries + 1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    m_zlink = BrowserApplication::zlink(defaultMaxNumberEntries - 1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    m_zlink = BrowserApplication::zlink(defaultMaxNumberEntries);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    m_zlink = BrowserApplication::zlink(-1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    m_manager->addHistoryEntry("http://google.com");

    m_zlink = BrowserApplication::zlink();
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 3);

    m_zlink = BrowserApplication::zlink(2);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    m_zlink = BrowserApplication::zlink(1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 1);

    m_zlink = BrowserApplication::zlink(-1);
    last = m_zlink->mostVisitedEntries();
    QCOMPARE(last.size(), 3);
}

void tst_zlink::mostVisitedEntriesHTML()
{
    QCOMPARE(m_zlink->mostVisitedEntriesHTML().compare("<p>No recent websites</p>"), 0);

    // m_zlink must contain http://twitter.com and http://facebook.com
    // that are two entries.
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_manager->addHistoryEntry("http://facebook.com/asd");
    m_manager->addHistoryEntry("http://facebook.com/poi");
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_manager->addHistoryEntry("http://twitter.com/oki");

    m_zlink->calculate();
    QVERIFY(m_zlink->mostVisitedEntriesHTML().compare("<p>No recent websites</p>") < 0);

}


void tst_zlink::mostVisitedEntriesConsistency()
{
    int nLast;
    int rowCount;

    nLast = m_zlink->mostVisitedEntries().size();
    rowCount = m_model->rowCount();
    QVERIFY((nLast == 0) && rowCount == 0);

    // m_zlink and m_model must contain http://twitter.com and http://facebook.com
    // that are two entries.
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_manager->addHistoryEntry("http://facebook.com/asd");
    m_manager->addHistoryEntry("http://facebook.com/poi");
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_manager->addHistoryEntry("http://twitter.com/oki");

    m_zlink->calculate();
    nLast = m_zlink->mostVisitedEntries().size();
    rowCount = m_model->rowCount();

    QVERIFY((nLast == 2) && rowCount == 2);

    m_manager->addHistoryEntry("http://google.com");

    nLast = m_zlink->mostVisitedEntries().size();
    rowCount = m_model->rowCount();

    // calculate() not yet forced!
    QVERIFY((nLast == 2) && rowCount == 3);

    m_zlink->calculate();
    nLast = m_zlink->mostVisitedEntries().size();
    QVERIFY((nLast == 3) && rowCount == 3);
}

void tst_zlink::render()
{
    m_zlink = BrowserApplication::zlink(0);
    QVERIFY(m_zlink->render().isEmpty() == false);
    m_zlink = BrowserApplication::zlink(5);
    QVERIFY(m_zlink->render().isEmpty() == false);
    m_zlink = BrowserApplication::zlink(15);
    QVERIFY(m_zlink->render().isEmpty() == false);
    m_zlink = BrowserApplication::zlink(-1);
    QVERIFY(m_zlink->render().isEmpty() == false);
}


void tst_zlink::verifyOperatorFrecencies()
{
    HistoryFrecencyEntry twitter(QString("http://twitter.com"), QDateTime(), QString("Twitter"), 100);
    HistoryFrecencyEntry facebook(QString("http://facebook.com"), QDateTime(), QString("Twitter"), 200);
    HistoryFrecencyEntry google(QString("http://google.com"), QDateTime(), QString("Google"), 500);
    HistoryFrecencyEntry yahoo(QString("http://yahoo.com"), QDateTime(), QString("Yahoo"), 200);

    QCOMPARE((twitter < facebook), true);
    QCOMPARE((twitter > facebook), false);
    QCOMPARE((twitter == facebook), false);

    QCOMPARE((facebook == yahoo), true);
    QCOMPARE((facebook > yahoo), false);
    QCOMPARE((google > facebook), true);
}


void tst_zlink::verifyFilterFrecencies()
{
    m_zlink = BrowserApplication::zlink(-1);

    QVERIFY(m_manager->history().count() == 0);
    m_manager->addHistoryEntry("http://facebook.com/lol");
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_manager->addHistoryEntry("http://facebook.com/asd");
    m_manager->addHistoryEntry("http://twitter.com/abc");
    m_manager->addHistoryEntry("http://twitter.com/def");
    m_manager->addHistoryEntry("http://twitter.com/def");
    m_manager->addHistoryEntry("http://twitter.com/qwe");
    m_manager->addHistoryEntry("http://twitter.com/def");
    m_manager->addHistoryEntry("http://twitter.com/poi");
    m_manager->addHistoryEntry("http://facebook.com/ooo");
    QVERIFY(m_manager->history().count() == 10);
    // at this point, we espect two results from m_zLinkFiltermodel
    // with the domains twitter.com as most visited and facebook.com as second one
    QVERIFY(m_model->rowCount() == 2);

    QVERIFY(m_model->historyLocation("http://facebook.com/ooo") > 0);

    m_zlink->calculate();
    QList<HistoryFrecencyEntry> mostVisited = m_zlink->mostVisitedEntries();
    QVERIFY(mostVisited.count() == 2);
    QVERIFY(mostVisited.first().url.compare(QString("http://twitter.com")) == 0);
    QVERIFY(mostVisited.last().url.compare(QString("http://facebook.com")) == 0);

}

void tst_zlink::verifyFaults()
{
    QVERIFY(m_zlink->mostVisitedEntries().size() == 0);
    m_manager->addHistoryEntry(" ");
    m_manager->addHistoryEntry("");
    m_manager->addHistoryEntry("qrc:/home.html");
    m_manager->addHistoryEntry("about:home");
    m_manager->addHistoryEntry("fake data");
    m_zlink->calculate();
    QVERIFY(m_zlink->mostVisitedEntries().size() == 0);

    //valid index to first entry
    QModelIndex index = m_model->index(0, 0, QModelIndex());
    QString url1(index.data(HistoryModel::UrlRole).toString());

    QModelIndex idx = m_model->mapToSource(index);
    QModelIndex idx2 = m_model->mapFromSource(idx);
    QString url2(idx2.data(HistoryModel::UrlRole).toString());

    QVERIFY(url1.compare(url2) == 0);

    //invalid index
    QCOMPARE(m_model->columnCount(QModelIndex()), 2);
    QCOMPARE(m_model->rowCount(idx), 0);
    QCOMPARE(m_model->data(m_model->index(-1, 1, QModelIndex()), zLinkFilterModel::FrecencyRole).toInt(), 0);
    QCOMPARE(m_model->data(m_model->index(-1, -1, QModelIndex()), zLinkFilterModel::FrecencyRole).toInt(), 0);
    QCOMPARE(m_model->data(m_model->index(1, -1, QModelIndex()), zLinkFilterModel::FrecencyRole).toInt(), 0);
    QCOMPARE(m_model->data(m_model->index(m_model->rowCount() + 1, -1, QModelIndex()), zLinkFilterModel::FrecencyRole).toInt(), 0);
    QCOMPARE(m_model->data(m_model->index(0, 100, QModelIndex()), zLinkFilterModel::FrecencyRole).toInt(), 0);
    QCOMPARE(m_model->historyLocation("http://notexistant.com"), 0);
    QCOMPARE(m_model->historyContains("http://notexistant.com"), 0);
    m_model->removeRows(-1, 0);
    m_model->removeRows(0, -1);
    m_model->removeRows(m_model->rowCount() + 1, 2);
    m_model->removeRows(0, 100);
    m_model->removeRows(-1, -1, index);
    QVERIFY(m_zlink->mostVisitedEntries().size() == 0);
    m_model->setSourceModel(m_model->sourceModel());

}

void tst_zlink::testMinorFilterMethods()
{
    QVERIFY(m_manager->history().count() == 0);
    QVERIFY(m_model->rowCount() == 0);
    m_manager->addHistoryEntry("http://facebook.com/lol");
    m_manager->addHistoryEntry("http://twitter.com/xyz");
    m_model->recalculateFrecencies();
    QVERIFY(m_manager->history().count() == 2);
    QVERIFY(m_model->rowCount() == 2);

    // this is twitter.com/xyz
    QModelIndex index_twitter = m_model->index(0, 0, QModelIndex());
    QModelIndex index_facebook = m_model->index(1, 0, QModelIndex());

    QString url_twitter(index_twitter.data(HistoryModel::UrlRole).toString());
    QString url_facebook(index_facebook.data(HistoryModel::UrlRole).toString());

    QVERIFY(url_twitter.compare(QString::fromLatin1("http://twitter.com/xyz")) == 0);
    QVERIFY(url_facebook.compare(QString::fromLatin1("http://facebook.com/lol")) == 0);

    QModelIndex fake1 = m_model->parent(index_facebook);
    QModelIndex fake2 = m_model->parent(index_twitter);

    QCOMPARE(fake1.internalId(), fake2.internalId());

    for(int i = 0; i < m_model->rowCount(); i++) {
        QModelIndex index = m_model->index(i, 0, QModelIndex());
        QCOMPARE(m_model->columnCount(index), 0);
        m_model->headerData(0, Qt::Horizontal, 0);
    }


    m_model->removeRows(0, 2);
    QVERIFY(m_manager->history().count() == 0);
    QVERIFY(m_model->rowCount() == 0);
    m_model->sourceModel();
    m_model->recalculateFrecencies();
}


QTEST_MAIN(tst_zlink)


