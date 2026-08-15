#include <QtTest>
#include "symbolswapengine.h"

using SymbolTypes = SymbolSwapEngine::SymbolTypes;

class TestSymbolSwapEngine : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void solvesKnownScenario();
    void resetClearsSteps();
    void getInstructionForStepBoundsAreSafe();
    void unsolvableCaseReportsNotSolved();

private:
    SymbolSwapEngine m_engine;
};

void TestSymbolSwapEngine::init()
{
    m_engine.reset();
}

// Same scenario as tst_calculatesteps.cpp's calculateStepsKnownScenario, expressed
// directly in terms of the pairs CalculateSteps would build: outer=(Wuerfel,
// Pyramide,Kugel) -> start pairs, inner=(Dreieck,Viereck,Kreis) -> target pairs.
void TestSymbolSwapEngine::solvesKnownScenario()
{
    SymbolSwapEngine::PairSet start = {{SymbolTypes::Viereck, SymbolTypes::Viereck},
                                        {SymbolTypes::Dreieck, SymbolTypes::Dreieck},
                                        {SymbolTypes::Kreis, SymbolTypes::Kreis}};
    SymbolSwapEngine::PairSet target = {{SymbolTypes::Viereck, SymbolTypes::Kreis},
                                         {SymbolTypes::Dreieck, SymbolTypes::Kreis},
                                         {SymbolTypes::Dreieck, SymbolTypes::Viereck}};

    m_engine.solve(start, target);

    QVERIFY(m_engine.isSolved());
    QCOMPARE(m_engine.numberOfSteps(), 2);

    QCOMPARE(m_engine.getInstructionForStep(0, 0), SymbolTypes::Viereck);
    QCOMPARE(m_engine.getInstructionForStep(0, 1), SymbolTypes::Undefined);
    QCOMPARE(m_engine.getInstructionForStep(0, 2), SymbolTypes::Kreis);

    QCOMPARE(m_engine.getInstructionForStep(1, 0), SymbolTypes::Undefined);
    QCOMPARE(m_engine.getInstructionForStep(1, 1), SymbolTypes::Dreieck);
    QCOMPARE(m_engine.getInstructionForStep(1, 2), SymbolTypes::Kreis);
}

void TestSymbolSwapEngine::resetClearsSteps()
{
    SymbolSwapEngine::PairSet start = {{SymbolTypes::Viereck, SymbolTypes::Viereck},
                                        {SymbolTypes::Dreieck, SymbolTypes::Dreieck},
                                        {SymbolTypes::Kreis, SymbolTypes::Kreis}};
    SymbolSwapEngine::PairSet target = {{SymbolTypes::Viereck, SymbolTypes::Kreis},
                                         {SymbolTypes::Dreieck, SymbolTypes::Kreis},
                                         {SymbolTypes::Dreieck, SymbolTypes::Viereck}};
    m_engine.solve(start, target);
    QVERIFY(m_engine.numberOfSteps() > 0);

    m_engine.reset();
    QCOMPARE(m_engine.numberOfSteps(), 0);
    QVERIFY(!m_engine.isSolved());
}

void TestSymbolSwapEngine::getInstructionForStepBoundsAreSafe()
{
    SymbolSwapEngine::PairSet start = {{SymbolTypes::Viereck, SymbolTypes::Viereck},
                                        {SymbolTypes::Dreieck, SymbolTypes::Dreieck},
                                        {SymbolTypes::Kreis, SymbolTypes::Kreis}};
    SymbolSwapEngine::PairSet target = {{SymbolTypes::Viereck, SymbolTypes::Kreis},
                                         {SymbolTypes::Dreieck, SymbolTypes::Kreis},
                                         {SymbolTypes::Dreieck, SymbolTypes::Viereck}};
    m_engine.solve(start, target);
    const int n = m_engine.numberOfSteps();

    QCOMPARE(m_engine.getInstructionForStep(-1, 0), SymbolTypes::Undefined);
    QCOMPARE(m_engine.getInstructionForStep(n, 0), SymbolTypes::Undefined);
}

// A 3-node set that can never reach a target with a totally disjoint symbol
// (Undefined is never a valid swap-in candidate since no node ever holds it as
// a "spare") must leave isSolved() false and must not hang.
void TestSymbolSwapEngine::unsolvableCaseReportsNotSolved()
{
    SymbolSwapEngine::PairSet start = {{SymbolTypes::Viereck, SymbolTypes::Viereck},
                                        {SymbolTypes::Dreieck, SymbolTypes::Dreieck},
                                        {SymbolTypes::Kreis, SymbolTypes::Kreis}};
    SymbolSwapEngine::PairSet target = {{SymbolTypes::Undefined, SymbolTypes::Undefined},
                                         {SymbolTypes::Dreieck, SymbolTypes::Kreis},
                                         {SymbolTypes::Dreieck, SymbolTypes::Viereck}};

    m_engine.solve(start, target);

    QVERIFY(!m_engine.isSolved());
}

QTEST_MAIN(TestSymbolSwapEngine)
#include "tst_symbolswapengine.moc"
