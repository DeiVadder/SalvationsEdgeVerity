#include <QtTest>
#include "challenge/encounterprogress.h"

class TestEncounterProgress : public QObject
{
    Q_OBJECT
private:
    EncounterProgress *m_progress = nullptr;

private slots:
    void init();
    void cleanup();

    void freshInstanceHasAllShapesAvailable();
    void markShapeUsedRemovesItFromAvailable();
    void markingUntrackedShapeIsNoOp();
    void allSixUsedWrapsToFreshCycle();
    void resetEncounterClearsUsedShapes();
    void challengeModeEnabledNotifies();
};

void TestEncounterProgress::init()
{
    m_progress = new EncounterProgress(this);
}

void TestEncounterProgress::cleanup()
{
    delete m_progress;
    m_progress = nullptr;
}

void TestEncounterProgress::freshInstanceHasAllShapesAvailable()
{
    QCOMPARE(m_progress->usedShapeCount(), 0);
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Kegel));
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Wuerfel));
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Zylinder));
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Pyramide));
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Prisma));
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Kugel));
    QCOMPARE(m_progress->availableShapes().size(), 6);
}

void TestEncounterProgress::markShapeUsedRemovesItFromAvailable()
{
    QSignalSpy spy(m_progress, &EncounterProgress::usedShapesChanged);
    m_progress->markShapeUsed(CalculateSteps::Kegel);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(m_progress->usedShapeCount(), 1);
    QVERIFY(!m_progress->isShapeAvailable(CalculateSteps::Kegel));
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Wuerfel));
    QCOMPARE(m_progress->availableShapes().size(), 5);
}

void TestEncounterProgress::markingUntrackedShapeIsNoOp()
{
    QSignalSpy spy(m_progress, &EncounterProgress::usedShapesChanged);
    m_progress->markShapeUsed(CalculateSteps::Dreieck);
    m_progress->markShapeUsed(CalculateSteps::Undefined);

    QCOMPARE(spy.count(), 0);
    QCOMPARE(m_progress->usedShapeCount(), 0);
}

void TestEncounterProgress::allSixUsedWrapsToFreshCycle()
{
    m_progress->markShapeUsed(CalculateSteps::Kegel);
    m_progress->markShapeUsed(CalculateSteps::Wuerfel);
    m_progress->markShapeUsed(CalculateSteps::Zylinder);
    m_progress->markShapeUsed(CalculateSteps::Pyramide);
    m_progress->markShapeUsed(CalculateSteps::Prisma);
    m_progress->markShapeUsed(CalculateSteps::Kugel);
    QCOMPARE(m_progress->usedShapeCount(), 6);
    QVERIFY(!m_progress->isShapeAvailable(CalculateSteps::Kegel));

    // The 7th mark starts a new cycle: everything but the just-marked shape
    // becomes available again.
    m_progress->markShapeUsed(CalculateSteps::Kegel);
    QCOMPARE(m_progress->usedShapeCount(), 1);
    QVERIFY(!m_progress->isShapeAvailable(CalculateSteps::Kegel));
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Wuerfel));
}

void TestEncounterProgress::resetEncounterClearsUsedShapes()
{
    m_progress->markShapeUsed(CalculateSteps::Kegel);
    m_progress->markShapeUsed(CalculateSteps::Wuerfel);
    QCOMPARE(m_progress->usedShapeCount(), 2);

    QSignalSpy spy(m_progress, &EncounterProgress::usedShapesChanged);
    m_progress->resetEncounter();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(m_progress->usedShapeCount(), 0);
    QVERIFY(m_progress->isShapeAvailable(CalculateSteps::Kegel));
}

void TestEncounterProgress::challengeModeEnabledNotifies()
{
    QVERIFY(!m_progress->challengeModeEnabled());

    QSignalSpy spy(m_progress, &EncounterProgress::challengeModeEnabledChanged);
    m_progress->setChallengeModeEnabled(true);
    QVERIFY(m_progress->challengeModeEnabled());
    QCOMPARE(spy.count(), 1);

    // Setting the same value again must not re-notify.
    m_progress->setChallengeModeEnabled(true);
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(TestEncounterProgress)
#include "tst_encounterprogress.moc"
