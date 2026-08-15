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

    void checkIsValidChallenge_data();
    void checkIsValidChallenge();
    void calculateStepsChallengeConvergesToOuterTargets();
    void calculateStepsChallengeHandlesPureDouble();
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

void TestCalculateInsideSteps::checkIsValidChallenge_data()
{
    QTest::addColumn<CalculateSteps::SymbolTypes>("t1");
    QTest::addColumn<CalculateSteps::SymbolTypes>("t2");
    QTest::addColumn<CalculateSteps::SymbolTypes>("t3");
    QTest::addColumn<bool>("expectedValid");

    // Kugel={Kreis,Kreis}, Prisma={Viereck,Dreieck} twice -> Kreis:2,
    // Viereck:2, Dreieck:2. Balanced, includes one pure double.
    QTest::newRow("balanced, one pure double")
        << CalculateSteps::Kugel << CalculateSteps::Prisma << CalculateSteps::Prisma << true;
    // Kegel={Dreieck,Kreis}, Zylinder={Kreis,Viereck}, Prisma={Viereck,Dreieck}
    // -> Dreieck:2, Kreis:2, Viereck:2. Balanced, no pure doubles (mirrors
    // the default/non-challenge case).
    QTest::newRow("balanced, all mixed")
        << CalculateSteps::Kegel << CalculateSteps::Zylinder << CalculateSteps::Prisma << true;
    // All three Kugel -> Kreis:6, Dreieck:0, Viereck:0. Unbalanced.
    QTest::newRow("unbalanced, all same pure double")
        << CalculateSteps::Kugel << CalculateSteps::Kugel << CalculateSteps::Kugel << false;
    QTest::newRow("undefined target slot")
        << CalculateSteps::Kugel << CalculateSteps::Undefined << CalculateSteps::Prisma << false;
}

void TestCalculateInsideSteps::checkIsValidChallenge()
{
    QFETCH(CalculateSteps::SymbolTypes, t1);
    QFETCH(CalculateSteps::SymbolTypes, t2);
    QFETCH(CalculateSteps::SymbolTypes, t3);
    QFETCH(bool, expectedValid);

    QCOMPARE(m_calc->checkIsValidChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                            CalculateSteps::Kreis, t1, t2, t3),
             expectedValid);
}

// Same self-consistency check as calculateStepsConvergesToFromBaseSymbolTarget,
// but with challenge targets that carry no pure double (should behave the
// same as the default target formula in substance, just entered explicitly).
void TestCalculateInsideSteps::calculateStepsChallengeConvergesToOuterTargets()
{
    m_calc->calculateStepsChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                     CalculateSteps::Kreis, CalculateSteps::Kegel,
                                     CalculateSteps::Zylinder, CalculateSteps::Prisma);

    QVERIFY(m_calc->isSolved());
    QVERIFY(m_calc->numberOfSteps() > 0);
    QCOMPARE(m_calc->finalShapeForPlayer(0), CalculateSteps::Kegel);
    QCOMPARE(m_calc->finalShapeForPlayer(1), CalculateSteps::Zylinder);
    QCOMPARE(m_calc->finalShapeForPlayer(2), CalculateSteps::Prisma);
}

// The actual point of challenge mode: one target is a pure double
// (Kugel={Kreis,Kreis}), so player 0 must end up with both Kreis copies
// instead of the default one-each split.
void TestCalculateInsideSteps::calculateStepsChallengeHandlesPureDouble()
{
    QVERIFY(m_calc->checkIsValidChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                           CalculateSteps::Kreis, CalculateSteps::Kugel,
                                           CalculateSteps::Prisma, CalculateSteps::Prisma));

    m_calc->calculateStepsChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                     CalculateSteps::Kreis, CalculateSteps::Kugel,
                                     CalculateSteps::Prisma, CalculateSteps::Prisma);

    QVERIFY(m_calc->isSolved());
    QCOMPARE(m_calc->finalShapeForPlayer(0), CalculateSteps::Kugel);
    QCOMPARE(m_calc->finalShapeForPlayer(1), CalculateSteps::Prisma);
    QCOMPARE(m_calc->finalShapeForPlayer(2), CalculateSteps::Prisma);
}

QTEST_MAIN(TestCalculateInsideSteps)
#include "tst_calculateinsidesteps.moc"
