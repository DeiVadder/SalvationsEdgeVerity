#include <QtTest>
#include "calculatesteps.h"

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

    void getInstructionForStepOffByOne();
    void getInstructionForStepNegativeIndex();
    void getInstructionForStepValidRange();

    void calculateStepsWithInvalidInputDoesNotHang();

    void targetShapeForStatueMatchesFromBaseSymbol();
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
            if (m_calc->getInstructionForStep(step, statue) != CalculateSteps::Undefined)
                ++nonUndefinedCount;
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

QTEST_MAIN(TestCalculateSteps)
#include "tst_calculatesteps.moc"
