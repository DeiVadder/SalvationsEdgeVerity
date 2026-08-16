#include <QtTest>
#include "puzzle/calculatesteps.h"

class TestCalculateSteps : public QObject
{
    Q_OBJECT
private:
    CalculateSteps *m_calc = nullptr;

private slots:
    void init();
    void cleanup();

    void resetClearsSteps();

    void checkIsValid_data();
    void checkIsValid();

    void calculateStepsKnownScenario();
    void shapeAfterStepMatchesHandTrace();

    void getInstructionForStepOffByOne();
    void getInstructionForStepNegativeIndex();
    void getInstructionForStepValidRange();

    void calculateStepsWithInvalidInputDoesNotHang();

    void targetShapeForStatueMatchesFromBaseSymbol();
    void isSolvedReflectsCalculationState();
    void calculationVersionBumpsOnEveryCall();

    void checkIsValidChallenge_data();
    void checkIsValidChallenge();
    void calculateStepsChallengeConvergesToDefaultEquivalentTarget();
    void calculateStepsChallengeHandlesPureDerangement();
};

void TestCalculateSteps::init()
{
    m_calc = new CalculateSteps(this);
}

void TestCalculateSteps::cleanup()
{
    delete m_calc;
    m_calc = nullptr;
}

void TestCalculateSteps::resetClearsSteps()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
    QVERIFY(m_calc->numberOfSteps() > 0);

    QSignalSpy spy(m_calc, &CalculateSteps::numberOfStepsChanged);
    m_calc->reset();
    QCOMPARE(m_calc->numberOfSteps(), 0);
    QCOMPARE(spy.count(), 1);
}

void TestCalculateSteps::checkIsValid_data()
{
    QTest::addColumn<CalculateSteps::SymbolTypes>("inner1");
    QTest::addColumn<CalculateSteps::SymbolTypes>("inner2");
    QTest::addColumn<CalculateSteps::SymbolTypes>("inner3");
    QTest::addColumn<CalculateSteps::SymbolTypes>("outer1");
    QTest::addColumn<CalculateSteps::SymbolTypes>("outer2");
    QTest::addColumn<CalculateSteps::SymbolTypes>("outer3");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("valid combo")
        << CalculateSteps::Dreieck << CalculateSteps::Viereck << CalculateSteps::Kreis
        << CalculateSteps::Wuerfel << CalculateSteps::Pyramide << CalculateSteps::Kugel
        << true;
    QTest::newRow("undefined outer slot")
        << CalculateSteps::Dreieck << CalculateSteps::Viereck << CalculateSteps::Kreis
        << CalculateSteps::Undefined << CalculateSteps::Pyramide << CalculateSteps::Kugel
        << false;
    QTest::newRow("symbol count mismatch")
        << CalculateSteps::Dreieck << CalculateSteps::Dreieck << CalculateSteps::Kreis
        << CalculateSteps::Wuerfel << CalculateSteps::Pyramide << CalculateSteps::Kugel
        << false;
}

void TestCalculateSteps::checkIsValid()
{
    QFETCH(CalculateSteps::SymbolTypes, inner1);
    QFETCH(CalculateSteps::SymbolTypes, inner2);
    QFETCH(CalculateSteps::SymbolTypes, inner3);
    QFETCH(CalculateSteps::SymbolTypes, outer1);
    QFETCH(CalculateSteps::SymbolTypes, outer2);
    QFETCH(CalculateSteps::SymbolTypes, outer3);
    QFETCH(bool, expectedValid);

    QCOMPARE(m_calc->checkIsValid(inner1, inner2, inner3, outer1, outer2, outer3), expectedValid);
}

// Hand-traced against the actual algorithm (not guessed): inner=(Dreieck,Viereck,Kreis),
// outer=(Wuerfel,Pyramide,Kugel) resolves in exactly 2 swap steps.
void TestCalculateSteps::calculateStepsKnownScenario()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);

    QCOMPARE(m_calc->numberOfSteps(), 2);

    QCOMPARE(m_calc->getInstructionForStep(0, 0), CalculateSteps::Viereck);
    QCOMPARE(m_calc->getInstructionForStep(0, 1), CalculateSteps::Undefined);
    QCOMPARE(m_calc->getInstructionForStep(0, 2), CalculateSteps::Kreis);

    QCOMPARE(m_calc->getInstructionForStep(1, 0), CalculateSteps::Undefined);
    QCOMPARE(m_calc->getInstructionForStep(1, 1), CalculateSteps::Dreieck);
    QCOMPARE(m_calc->getInstructionForStep(1, 2), CalculateSteps::Kreis);
}

// Same scenario as calculateStepsKnownScenario(), hand-traced further:
// statue0/statue2 swap in step 0 (Viereck<->Kreis), landing both on
// {Viereck,Kreis}=Zylinder; statue1 stays Pyramide. Step 1 swaps
// statue1/statue2 (Dreieck<->Kreis), landing on the final Kegel/Prisma -
// statue0 is untouched in step 1, so it's still Zylinder, not yet its
// eventual different value if there were a 3rd step.
void TestCalculateSteps::shapeAfterStepMatchesHandTrace()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);

    QCOMPARE(m_calc->shapeAfterStep(0, 0), CalculateSteps::Zylinder);
    QCOMPARE(m_calc->shapeAfterStep(0, 1), CalculateSteps::Pyramide);
    QCOMPARE(m_calc->shapeAfterStep(0, 2), CalculateSteps::Zylinder);

    QCOMPARE(m_calc->shapeAfterStep(1, 0), CalculateSteps::Zylinder);
    QCOMPARE(m_calc->shapeAfterStep(1, 1), CalculateSteps::Kegel);
    QCOMPARE(m_calc->shapeAfterStep(1, 2), CalculateSteps::Prisma);

    // Final step's per-node state must match the overall target.
    QCOMPARE(m_calc->shapeAfterStep(1, 0), m_calc->targetShapeForStatue(0));
    QCOMPARE(m_calc->shapeAfterStep(1, 1), m_calc->targetShapeForStatue(1));
    QCOMPARE(m_calc->shapeAfterStep(1, 2), m_calc->targetShapeForStatue(2));

    QCOMPARE(m_calc->shapeAfterStep(-1, 0), CalculateSteps::Undefined);
    QCOMPARE(m_calc->shapeAfterStep(2, 0), CalculateSteps::Undefined);
}

void TestCalculateSteps::getInstructionForStepOffByOne()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
    const int n = m_calc->numberOfSteps();
    QVERIFY(n > 0);
    QCOMPARE(m_calc->getInstructionForStep(n, 0), CalculateSteps::Undefined);
}

void TestCalculateSteps::getInstructionForStepNegativeIndex()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
    QCOMPARE(m_calc->getInstructionForStep(-1, 0), CalculateSteps::Undefined);
}

void TestCalculateSteps::getInstructionForStepValidRange()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
    const int n = m_calc->numberOfSteps();
    QVERIFY(n > 0);
    for (int step = 0; step < n; ++step) {
        int nonUndefinedCount = 0;
        for (int statue = 0; statue < 3; ++statue) {
            if (m_calc->getInstructionForStep(step, statue) != CalculateSteps::Undefined) {
                ++nonUndefinedCount;
            }
        }
        QCOMPARE(nonUndefinedCount, 2);
    }
}

void TestCalculateSteps::calculateStepsWithInvalidInputDoesNotHang()
{
    QVERIFY(!m_calc->checkIsValid(CalculateSteps::Dreieck, CalculateSteps::Dreieck, CalculateSteps::Kreis,
                                   CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel));
    // Feeding this checkIsValid()-rejected combination directly into calculateSteps() must not
    // hang; returning at all is the assertion.
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Dreieck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
}

void TestCalculateSteps::targetShapeForStatueMatchesFromBaseSymbol()
{
    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);

    // fromBaseSymbol(Dreieck) = {Viereck, Kreis} -> Zylinder
    QCOMPARE(m_calc->targetShapeForStatue(0), CalculateSteps::Zylinder);
    // fromBaseSymbol(Viereck) = {Dreieck, Kreis} -> Kegel
    QCOMPARE(m_calc->targetShapeForStatue(1), CalculateSteps::Kegel);
    // fromBaseSymbol(Kreis) = {Dreieck, Viereck} -> Prisma
    QCOMPARE(m_calc->targetShapeForStatue(2), CalculateSteps::Prisma);

    QCOMPARE(m_calc->targetShapeForStatue(3), CalculateSteps::Undefined);
    QCOMPARE(m_calc->targetShapeForStatue(-1), CalculateSteps::Undefined);

    m_calc->reset();
    QCOMPARE(m_calc->targetShapeForStatue(0), CalculateSteps::Undefined);
}

void TestCalculateSteps::isSolvedReflectsCalculationState()
{
    QVERIFY(!m_calc->isSolved());

    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
    QVERIFY(m_calc->isSolved());

    m_calc->reset();
    QVERIFY(!m_calc->isSolved());
}

// calculationVersion exists specifically so QML bindings that only call
// plain Q_INVOKABLE methods (getInstructionForStep(), targetShapeForStatue())
// have a reliable NOTIFY-backed trigger to depend on - unlike numberOfSteps,
// which can legitimately stay the same across two different calculations
// with different actual results, this counter must strictly increase on
// every single calculateSteps()/reset() call, with no exceptions.
void TestCalculateSteps::calculationVersionBumpsOnEveryCall()
{
    QSignalSpy spy(m_calc, &CalculateSteps::calculationVersionChanged);
    int previous = m_calc->calculationVersion();

    m_calc->calculateSteps(CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
    QCOMPARE(m_calc->calculationVersion(), previous + 1);
    previous = m_calc->calculationVersion();

    // A second call, different scenario, regardless of whether the result
    // happens to share the same step count as the first - the version must
    // bump unconditionally either way.
    m_calc->calculateSteps(CalculateSteps::Viereck, CalculateSteps::Dreieck, CalculateSteps::Kreis,
                            CalculateSteps::Wuerfel, CalculateSteps::Pyramide, CalculateSteps::Kugel);
    QCOMPARE(m_calc->calculationVersion(), previous + 1);
    previous = m_calc->calculationVersion();

    m_calc->reset();
    QCOMPARE(m_calc->calculationVersion(), previous + 1);

    QCOMPARE(spy.count(), 3);
}

// Same own-symbol/derangement reasoning as CalculateInsideSteps'
// checkIsValidChallenge_data (see that file's comment): a target can never
// include the statue's own inner symbol, which pins each of the 3 mixed
// shapes to exactly one valid statue and forces any pure-shape usage into
// a full 3-way derangement.
void TestCalculateSteps::checkIsValidChallenge_data()
{
    QTest::addColumn<CalculateSteps::SymbolTypes>("t1");
    QTest::addColumn<CalculateSteps::SymbolTypes>("t2");
    QTest::addColumn<CalculateSteps::SymbolTypes>("t3");
    QTest::addColumn<bool>("expectedValid");

    // Wuerfel={Viereck,Viereck} at statue0 (inner Dreieck), Kugel={Kreis,Kreis}
    // at statue1 (inner Viereck), Pyramide={Dreieck,Dreieck} at statue2
    // (inner Kreis) - balanced, none at the statue matching its own symbol.
    QTest::newRow("balanced, pure derangement")
        << CalculateSteps::Wuerfel << CalculateSteps::Kugel << CalculateSteps::Pyramide << true;
    // Exactly the default fromBaseSymbol(inner) split, entered explicitly.
    QTest::newRow("balanced, all mixed")
        << CalculateSteps::Zylinder << CalculateSteps::Kegel << CalculateSteps::Prisma << true;
    QTest::newRow("unbalanced")
        << CalculateSteps::Kugel << CalculateSteps::Kugel << CalculateSteps::Kugel << false;
    QTest::newRow("undefined target slot")
        << CalculateSteps::Kugel << CalculateSteps::Undefined << CalculateSteps::Pyramide << false;
    // Kegel={Dreieck,Kreis} at statue0 (inner Dreieck) - contains its own
    // symbol, mechanically impossible (a statue can only give away/hold
    // copies of its own symbol, never receive one back).
    QTest::newRow("target contains own symbol")
        << CalculateSteps::Kegel << CalculateSteps::Kugel << CalculateSteps::Pyramide << false;
}

void TestCalculateSteps::checkIsValidChallenge()
{
    QFETCH(CalculateSteps::SymbolTypes, t1);
    QFETCH(CalculateSteps::SymbolTypes, t2);
    QFETCH(CalculateSteps::SymbolTypes, t3);
    QFETCH(bool, expectedValid);

    QCOMPARE(m_calc->checkIsValidChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                            CalculateSteps::Kreis, t1, t2, t3),
             expectedValid);
}

void TestCalculateSteps::calculateStepsChallengeConvergesToDefaultEquivalentTarget()
{
    QVERIFY(m_calc->checkIsValidChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                           CalculateSteps::Kreis, CalculateSteps::Zylinder,
                                           CalculateSteps::Kegel, CalculateSteps::Prisma));

    m_calc->calculateStepsChallenge(CalculateSteps::Wuerfel, CalculateSteps::Pyramide,
                                     CalculateSteps::Kugel, CalculateSteps::Zylinder,
                                     CalculateSteps::Kegel, CalculateSteps::Prisma);

    QVERIFY(m_calc->isSolved());
    QCOMPARE(m_calc->targetShapeForStatue(0), CalculateSteps::Zylinder);
    QCOMPARE(m_calc->targetShapeForStatue(1), CalculateSteps::Kegel);
    QCOMPARE(m_calc->targetShapeForStatue(2), CalculateSteps::Prisma);
}

// The actual point of challenge mode: a pure-shape derangement instead of
// the default one-each mixed split.
void TestCalculateSteps::calculateStepsChallengeHandlesPureDerangement()
{
    QVERIFY(m_calc->checkIsValidChallenge(CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                           CalculateSteps::Kreis, CalculateSteps::Wuerfel,
                                           CalculateSteps::Kugel, CalculateSteps::Pyramide));

    m_calc->calculateStepsChallenge(CalculateSteps::Wuerfel, CalculateSteps::Pyramide,
                                     CalculateSteps::Kugel, CalculateSteps::Wuerfel,
                                     CalculateSteps::Kugel, CalculateSteps::Pyramide);

    QVERIFY(m_calc->isSolved());
    QCOMPARE(m_calc->targetShapeForStatue(0), CalculateSteps::Wuerfel);
    QCOMPARE(m_calc->targetShapeForStatue(1), CalculateSteps::Kugel);
    QCOMPARE(m_calc->targetShapeForStatue(2), CalculateSteps::Pyramide);
}

QTEST_MAIN(TestCalculateSteps)
#include "tst_calculatesteps.moc"
