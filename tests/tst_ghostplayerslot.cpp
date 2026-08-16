#include <QtTest>
#include "ghost/ghostplayerslot.h"

class TestGhostPlayerSlot : public QObject
{
    Q_OBJECT
private:
    GhostPlayerSlot *m_slot = nullptr;

private slots:
    void init();
    void cleanup();

    void defaultsAreEmpty();
    void settersNotifyOnChange();
    void settersDoNotNotifyOnSameValue();
    void resetClearsAllFields();
};

void TestGhostPlayerSlot::init()
{
    m_slot = new GhostPlayerSlot(this);
}

void TestGhostPlayerSlot::cleanup()
{
    delete m_slot;
    m_slot = nullptr;
}

void TestGhostPlayerSlot::defaultsAreEmpty()
{
    QCOMPARE(m_slot->playerName(), QString());
    QCOMPARE(m_slot->classType(), 0);
    QCOMPARE(m_slot->manualNote(), QString());
}

void TestGhostPlayerSlot::settersNotifyOnChange()
{
    QSignalSpy nameSpy(m_slot, &GhostPlayerSlot::playerNameChanged);
    QSignalSpy classSpy(m_slot, &GhostPlayerSlot::classTypeChanged);
    QSignalSpy noteSpy(m_slot, &GhostPlayerSlot::manualNoteChanged);

    m_slot->setPlayerName("Guardian1");
    m_slot->setClassType(2);
    m_slot->setManualNote("Crimson Rain shell");

    QCOMPARE(m_slot->playerName(), QStringLiteral("Guardian1"));
    QCOMPARE(m_slot->classType(), 2);
    QCOMPARE(m_slot->manualNote(), QStringLiteral("Crimson Rain shell"));
    QCOMPARE(nameSpy.count(), 1);
    QCOMPARE(classSpy.count(), 1);
    QCOMPARE(noteSpy.count(), 1);
}

void TestGhostPlayerSlot::settersDoNotNotifyOnSameValue()
{
    m_slot->setPlayerName("Guardian1");
    QSignalSpy nameSpy(m_slot, &GhostPlayerSlot::playerNameChanged);
    m_slot->setPlayerName("Guardian1");
    QCOMPARE(nameSpy.count(), 0);
}

void TestGhostPlayerSlot::resetClearsAllFields()
{
    m_slot->setPlayerName("Guardian1");
    m_slot->setClassType(1);
    m_slot->setManualNote("some note");

    m_slot->reset();

    QCOMPARE(m_slot->playerName(), QString());
    QCOMPARE(m_slot->classType(), 0);
    QCOMPARE(m_slot->manualNote(), QString());
}

QTEST_MAIN(TestGhostPlayerSlot)
#include "tst_ghostplayerslot.moc"
