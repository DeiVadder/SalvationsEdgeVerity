#include <QtTest>
#include "calculateinsidesteps.h"

class TestCalculateInsideSteps : public QObject
{
    Q_OBJECT
private:
    CalculateInsideSteps *m_calc = nullptr;

private slots:
    void init();
    void cleanup();

    void checkIsValid_data();
    void checkIsValid();

    void calculateStepsConvergesToFromBaseSymbolTarget();
    void finalShapeMatchesPairToShape();
    void resetClearsSteps();
};

void TestCalculateInsideSteps::init()
{
    m_calc = new CalculateInsideSteps(this);
}

void TestCalculateInsideSteps::cleanup()
{
    delete m_calc;
    m_calc = nullptr;
}

void TestCalculateInsideSteps::checkIsValid_data()
{
    QTest::addColumn<CalculateSteps::SymbolTypes>("p1");
    QTest::addColumn<CalculateSteps::SymbolTypes>("p2");
    QTest::addColumn<CalculateSteps::SymbolTypes>("p3");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("valid, pairwise distinct")
        << CalculateSteps::Dreieck << CalculateSteps::Viereck << CalculateSteps::Kreis << true;
    QTest::newRow("undefined slot")
        << CalculateSteps::Dreieck << CalculateSteps::Undefined << CalculateSteps::Kreis << false;
    QTest::newRow("3D shape instead of 2D symbol")
        << CalculateSteps::Dreieck << CalculateSteps::Viereck << CalculateSteps::Kegel << false;
    QTest::newRow("duplicate symbols")
        << CalculateSteps::Dreieck << CalculateSteps::Dreieck << CalculateSteps::Kreis << false;
}

void TestCalculateInsideSteps::checkIsValid()
{
    QFETCH(CalculateSteps::SymbolTypes, p1);
    QFETCH(CalculateSteps::SymbolTypes, p2);
    QFETCH(CalculateSteps::SymbolTypes, p3);
    QFETCH(bool, expectedValid);

    QCOMPARE(m_calc->checkIsValid(p1, p2, p3), expectedValid);
}

// The engine, given start={{p,p}} and target=fromBaseSymbol(p) per player,
// must actually reach that target (isSolved()==true) and every recorded
// step must only ever hand out symbols that are part of some player's
// target pair - i.e. the simulation is internally consistent, not just
// "doesn't crash". This does NOT verify the result matches real gameplay
// (see the "NOTE" in calculateinsidesteps.h) - only that the model's own
// math is self-consistent.
void TestCalculateInsideSteps::calculateStepsConvergesToFromBaseSymbolTarget()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis);

    QVERIFY(m_calc->isSolved());
    QVERIFY(m_calc->numberOfSteps() > 0);

    for (int step = 0; step < m_calc->numberOfSteps(); ++step) {
        int nonUndefinedCount = 0;
        for (int player = 0; player < 3; ++player) {
            if (m_calc->getInstructionForStep(step, player) != CalculateSteps::Undefined)
                ++nonUndefinedCount;
        }
        QCOMPARE(nonUndefinedCount, 2);
    }
}

void TestCalculateInsideSteps::finalShapeMatchesPairToShape()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis);
    QVERIFY(m_calc->isSolved());

    // fromBaseSymbol(Dreieck) = {Viereck, Kreis} -> Zylinder
    QCOMPARE(m_calc->finalShapeForPlayer(0), CalculateSteps::Zylinder);
    // fromBaseSymbol(Viereck) = {Dreieck, Kreis} -> Kegel
    QCOMPARE(m_calc->finalShapeForPlayer(1), CalculateSteps::Kegel);
    // fromBaseSymbol(Kreis) = {Dreieck, Viereck} -> Prisma
    QCOMPARE(m_calc->finalShapeForPlayer(2), CalculateSteps::Prisma);

    QCOMPARE(m_calc->finalShapeForPlayer(3), CalculateSteps::Undefined);
    QCOMPARE(m_calc->finalShapeForPlayer(-1), CalculateSteps::Undefined);
}

void TestCalculateInsideSteps::resetClearsSteps()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis);
    QVERIFY(m_calc->numberOfSteps() > 0);

    QSignalSpy spy(m_calc, &CalculateInsideSteps::numberOfStepsChanged);
    m_calc->reset();

    QCOMPARE(m_calc->numberOfSteps(), 0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(m_calc->finalShapeForPlayer(0), CalculateSteps::Undefined);
}

QTEST_MAIN(TestCalculateInsideSteps)
#include "tst_calculateinsidesteps.moc"
