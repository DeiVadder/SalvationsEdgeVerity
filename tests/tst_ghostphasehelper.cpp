#include <QtTest>
#include "ghost/ghostphasehelper.h"
#include "ghost/ghostplayerslot.h"

class TestGhostPhaseHelper : public QObject
{
    Q_OBJECT
private:
    GhostPhaseHelper *m_helper = nullptr;

private slots:
    void init();
    void cleanup();

    void rosterHasSixSlots();
    void resetClearsAllSlots();
};

void TestGhostPhaseHelper::init()
{
    m_helper = new GhostPhaseHelper(this);
}

void TestGhostPhaseHelper::cleanup()
{
    delete m_helper;
    m_helper = nullptr;
}

void TestGhostPhaseHelper::rosterHasSixSlots()
{
    const auto players = m_helper->players();
    QCOMPARE(players.size(), 6);
    for (auto *player : players)
        QVERIFY(qobject_cast<GhostPlayerSlot *>(player) != nullptr);
}

void TestGhostPhaseHelper::resetClearsAllSlots()
{
    const auto players = m_helper->players();
    for (int i = 0; i < players.size(); ++i) {
        auto *slot = qobject_cast<GhostPlayerSlot *>(players.at(i));
        slot->setPlayerName(QStringLiteral("Player%1").arg(i));
        slot->setManualNote(QStringLiteral("Note%1").arg(i));
    }

    m_helper->reset();

    for (auto *player : players) {
        auto *slot = qobject_cast<GhostPlayerSlot *>(player);
        QCOMPARE(slot->playerName(), QString());
        QCOMPARE(slot->manualNote(), QString());
    }
}

QTEST_MAIN(TestGhostPhaseHelper)
#include "tst_ghostphasehelper.moc"
