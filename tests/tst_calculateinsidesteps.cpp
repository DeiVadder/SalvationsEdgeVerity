#include <QtTest>

#include "puzzle/calculateinsidesteps.h"

namespace {
int countSymbol(const std::initializer_list<CalculateSteps::SymbolTypes> &walls,
                 CalculateSteps::SymbolTypes symbol)
{
    int count = 0;
    for (auto s : walls)
        if (s == symbol)
            ++count;
    return count;
}
} // namespace

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

    void checkIsValidWall_data();
    void checkIsValidWall();
    void checkIsValidWallExhaustive();
    void calculateStepsLFGConvergesInTwoPhases();
    void calculateStepsFastDelegatesToResolver();
    void calculationVersionBumpsOnEveryCallPath();
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

void TestCalculateInsideSteps::checkIsValidWall_data()
{
    QTest::addColumn<CalculateSteps::SymbolTypes>("w1a");
    QTest::addColumn<CalculateSteps::SymbolTypes>("w1b");
    QTest::addColumn<CalculateSteps::SymbolTypes>("w2a");
    QTest::addColumn<CalculateSteps::SymbolTypes>("w2b");
    QTest::addColumn<CalculateSteps::SymbolTypes>("w3a");
    QTest::addColumn<CalculateSteps::SymbolTypes>("w3b");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("balanced, each symbol twice")
        << CalculateSteps::Viereck << CalculateSteps::Kreis
        << CalculateSteps::Dreieck << CalculateSteps::Kreis
        << CalculateSteps::Dreieck << CalculateSteps::Viereck << true;
    QTest::newRow("unbalanced")
        << CalculateSteps::Dreieck << CalculateSteps::Dreieck
        << CalculateSteps::Dreieck << CalculateSteps::Dreieck
        << CalculateSteps::Kreis << CalculateSteps::Kreis << false;
    QTest::newRow("undefined wall slot")
        << CalculateSteps::Undefined << CalculateSteps::Kreis
        << CalculateSteps::Dreieck << CalculateSteps::Kreis
        << CalculateSteps::Dreieck << CalculateSteps::Viereck << false;
}

void TestCalculateInsideSteps::checkIsValidWall()
{
    QFETCH(CalculateSteps::SymbolTypes, w1a);
    QFETCH(CalculateSteps::SymbolTypes, w1b);
    QFETCH(CalculateSteps::SymbolTypes, w2a);
    QFETCH(CalculateSteps::SymbolTypes, w2b);
    QFETCH(CalculateSteps::SymbolTypes, w3a);
    QFETCH(CalculateSteps::SymbolTypes, w3b);
    QFETCH(bool, expectedValid);

    QCOMPARE(m_calc->checkIsValidWall(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                       CalculateSteps::Kreis, w1a, w1b, w2a, w2b, w3a, w3b),
             expectedValid);

    // Invalid player symbols reject regardless of wall balance.
    QVERIFY(!m_calc->checkIsValidWall(CalculateSteps::Dreieck, CalculateSteps::Dreieck,
                                       CalculateSteps::Kreis, w1a, w1b, w2a, w2b, w3a, w3b));
}

// Exhaustively checks checkIsValidWall()'s balance verdict against an
// independently-computed reference count, across every one of the 3^6 =
// 729 raw wall combinations (90 balanced, 639 unbalanced) - not just the
// handful of hand-picked rows above.
void TestCalculateInsideSteps::checkIsValidWallExhaustive()
{
    const QVector<CalculateSteps::SymbolTypes> symbols = {CalculateSteps::Dreieck,
                                                            CalculateSteps::Viereck,
                                                            CalculateSteps::Kreis};
    int testedCount = 0;
    int balancedCount = 0;

    for (auto w1a : symbols) for (auto w1b : symbols)
    for (auto w2a : symbols) for (auto w2b : symbols)
    for (auto w3a : symbols) for (auto w3b : symbols) {
        bool referenceBalanced =
            countSymbol({w1a, w1b, w2a, w2b, w3a, w3b}, CalculateSteps::Dreieck) == 2
            && countSymbol({w1a, w1b, w2a, w2b, w3a, w3b}, CalculateSteps::Viereck) == 2
            && countSymbol({w1a, w1b, w2a, w2b, w3a, w3b}, CalculateSteps::Kreis) == 2;

        bool actual = m_calc->checkIsValidWall(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                                CalculateSteps::Kreis, w1a, w1b, w2a, w2b, w3a, w3b);

        QCOMPARE(actual, referenceBalanced);
        ++testedCount;
        if (referenceBalanced)
            ++balancedCount;
    }

    QCOMPARE(testedCount, 729);
    QCOMPARE(balancedCount, 90);
}

// LFG phase 1 (cleanse: wall -> self-pair) and phase 2 (distribute:
// self-pair -> fromBaseSymbol(own), reusing the exact same target formula
// calculateSteps() uses) must both independently converge, and the final
// escape shapes must match the default (non-wall) path exactly - the wall
// input only changes the journey, not the destination.
void TestCalculateInsideSteps::calculateStepsLFGConvergesInTwoPhases()
{
    QVERIFY(m_calc->checkIsValidWall(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                      CalculateSteps::Kreis, CalculateSteps::Viereck,
                                      CalculateSteps::Kreis, CalculateSteps::Dreieck,
                                      CalculateSteps::Kreis, CalculateSteps::Dreieck,
                                      CalculateSteps::Viereck));

    m_calc->calculateStepsLFG(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                               CalculateSteps::Kreis, CalculateSteps::Viereck,
                               CalculateSteps::Kreis, CalculateSteps::Dreieck,
                               CalculateSteps::Kreis, CalculateSteps::Dreieck,
                               CalculateSteps::Viereck);

    QVERIFY(m_calc->isCleanseSolved());
    QVERIFY(m_calc->isSolved());
    QVERIFY(m_calc->numberOfCleanseSteps() >= 0);
    QVERIFY(m_calc->numberOfSteps() > 0);
    QCOMPARE(m_calc->finalShapeForPlayer(0), CalculateSteps::Zylinder);
    QCOMPARE(m_calc->finalShapeForPlayer(1), CalculateSteps::Kegel);
    QCOMPARE(m_calc->finalShapeForPlayer(2), CalculateSteps::Prisma);
}

// Fast delegates to FastCleanseResolver (unit-tested on its own in
// tst_fastcleanseresolver.cpp) - here just confirm the wiring/target
// formula is correct: same final shapes as the default/LFG path.
void TestCalculateInsideSteps::calculateStepsFastDelegatesToResolver()
{
    m_calc->calculateStepsFast(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                CalculateSteps::Kreis, CalculateSteps::Dreieck,
                                CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                CalculateSteps::Viereck, CalculateSteps::Kreis,
                                CalculateSteps::Kreis);

    QVERIFY(m_calc->isFastSolved());
    QCOMPARE(m_calc->numberOfFastRounds(), 1);
    QCOMPARE(m_calc->numberOfFastTransfers(), 6);
    QCOMPARE(m_calc->finalShapeForPlayer(0), CalculateSteps::Zylinder);
    QCOMPARE(m_calc->finalShapeForPlayer(1), CalculateSteps::Kegel);
    QCOMPARE(m_calc->finalShapeForPlayer(2), CalculateSteps::Prisma);
}

// calculationVersion must strictly increase on every single calculate*()/
// reset() call regardless of which internal engine it touches - this is
// what QML's Fast/LFG-mode-only bindings rely on for a reliable
// re-evaluation trigger (see InsideRoomPanel.qml and the class doc
// comment in calculateinsidesteps.h for why numberOfSteps alone wasn't
// sufficient: calculateStepsFast() never touches the engine numberOfSteps
// reads from).
void TestCalculateInsideSteps::calculationVersionBumpsOnEveryCallPath()
{
    QSignalSpy spy(m_calc, &CalculateInsideSteps::calculationVersionChanged);
    int previous = m_calc->calculationVersion();

    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis);
    QCOMPARE(m_calc->calculationVersion(), previous + 1);
    previous = m_calc->calculationVersion();

    m_calc->calculateStepsChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                     CalculateSteps::Kreis, CalculateSteps::Kegel,
                                     CalculateSteps::Zylinder, CalculateSteps::Prisma);
    QCOMPARE(m_calc->calculationVersion(), previous + 1);
    previous = m_calc->calculationVersion();

    m_calc->calculateStepsLFG(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                               CalculateSteps::Kreis, CalculateSteps::Viereck,
                               CalculateSteps::Kreis, CalculateSteps::Dreieck,
                               CalculateSteps::Kreis, CalculateSteps::Dreieck,
                               CalculateSteps::Viereck);
    QCOMPARE(m_calc->calculationVersion(), previous + 1);
    previous = m_calc->calculationVersion();

    // The exact case the fix targets: calculateStepsFast() never touches
    // m_engine (numberOfSteps's source), so only calculationVersion can
    // reliably signal that this call happened.
    m_calc->calculateStepsFast(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                CalculateSteps::Kreis, CalculateSteps::Dreieck,
                                CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                CalculateSteps::Viereck, CalculateSteps::Kreis,
                                CalculateSteps::Kreis);
    QCOMPARE(m_calc->calculationVersion(), previous + 1);
    previous = m_calc->calculationVersion();

    m_calc->reset();
    QCOMPARE(m_calc->calculationVersion(), previous + 1);

    QCOMPARE(spy.count(), 5);
}

QTEST_MAIN(TestCalculateInsideSteps)
#include "tst_calculateinsidesteps.moc"
