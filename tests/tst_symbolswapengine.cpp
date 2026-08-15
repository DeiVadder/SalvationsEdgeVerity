#include <QtTest>

#include <algorithm>

#include "puzzle/symbolswapengine.h"

using SymbolTypes = SymbolSwapEngine::SymbolTypes;
using PairSet = SymbolSwapEngine::PairSet;

namespace {

int countSymbol(const PairSet &pairs, SymbolTypes symbol)
{
    int count = 0;
    for (const auto &pair : pairs)
        count += pair.count(symbol);
    return count;
}

bool isBalanced(const PairSet &pairs)
{
    return countSymbol(pairs, SymbolTypes::Dreieck) == 2
        && countSymbol(pairs, SymbolTypes::Viereck) == 2
        && countSymbol(pairs, SymbolTypes::Kreis) == 2;
}

// Every 3-pair set where each of the 3 base symbols appears exactly
// twice across the 6 slots (checkIsValid()'s invariant, both for the
// outside puzzle's 3 statues and the inside puzzle's 3 wall pairs).
// There are 90 of these (multinomial 6!/(2!2!2!)).
QVector<PairSet> allBalancedPairSets()
{
    const QVector<SymbolTypes> symbols = {SymbolTypes::Dreieck, SymbolTypes::Viereck,
                                           SymbolTypes::Kreis};
    QVector<PairSet> result;
    for (auto a1a : symbols) for (auto a1b : symbols)
    for (auto a2a : symbols) for (auto a2b : symbols)
    for (auto a3a : symbols) for (auto a3b : symbols) {
        PairSet candidate = {{a1a, a1b}, {a2a, a2b}, {a3a, a3b}};
        if (isBalanced(candidate))
            result.append(candidate);
    }
    return result;
}

bool matchesSorted(QVector<SymbolTypes> a, QVector<SymbolTypes> b)
{
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

// Independently replays the engine's own recorded step instructions
// (exactly as StepCard.qml consumes them: per step, exactly 2 nodes are
// "active", each instructions[node] is the symbol THAT node hands to the
// other) against a fresh copy of `start` - an oracle that doesn't touch
// the engine's internal isSolved()/isFinished() bookkeeping at all.
PairSet replayInstructions(PairSet start, const SymbolSwapEngine &engine)
{
    for (int step = 0; step < engine.numberOfSteps(); ++step) {
        QVector<int> activeNodes;
        QVector<SymbolTypes> givenSymbols;
        for (int node = 0; node < start.size(); ++node) {
            auto instr = engine.getInstructionForStep(step, node);
            if (instr != SymbolTypes::Undefined) {
                activeNodes.append(node);
                givenSymbols.append(instr);
            }
        }
        if (activeNodes.size() != 2)
            return start; // malformed step - let the caller's comparison fail
        int a = activeNodes.at(0), b = activeNodes.at(1);
        start[a].removeOne(givenSymbols.at(0));
        start[a].append(givenSymbols.at(1));
        start[b].removeOne(givenSymbols.at(1));
        start[b].append(givenSymbols.at(0));
    }
    return start;
}

} // namespace

class TestSymbolSwapEngine : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void solvesKnownScenario();
    void resetClearsSteps();
    void getInstructionForStepBoundsAreSafe();
    void unsolvableCaseReportsNotSolved();

    void allBalancedStartTargetPairsConverge();
    void unbalancedTargetNeverFalselyClaimsSuccess();

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

// Exhaustively covers every one of the 90 x 90 = 8100 (balanced start,
// balanced target) pairs - every combination the outside dissection
// puzzle, the LFG cleanse phase, and the LFG distribute phase could ever
// actually hand this engine. For each: the engine must converge
// (isSolved()==true), AND an INDEPENDENT replay of its own recorded
// step instructions (not touching isSolved()'s internal bookkeeping)
// must land exactly on the target.
void TestSymbolSwapEngine::allBalancedStartTargetPairsConverge()
{
    const QVector<PairSet> balancedSets = allBalancedPairSets();
    QCOMPARE(balancedSets.size(), 90);

    int tested = 0;
    int maxSteps = 0;

    for (const auto &start : balancedSets) {
        for (const auto &target : balancedSets) {
            SymbolSwapEngine engine;
            engine.solve(start, target);
            ++tested;

            QVERIFY(engine.isSolved());

            auto replayed = replayInstructions(start, engine);
            for (int node = 0; node < 3; ++node)
                QVERIFY(matchesSorted(replayed.at(node), target.at(node)));

            maxSteps = qMax(maxSteps, engine.numberOfSteps());
        }
    }

    qDebug() << "Exhaustively verified" << tested
             << "balanced (start x target) pairs, max steps seen:" << maxSteps;
    QCOMPARE(tested, 90 * 90);
}

// Complementary "expect fail" sweep: pairing every balanced start with
// every UNBALANCED target (unreachable by construction - swaps only move
// existing symbols, they can't change the total count of each symbol)
// must never be reported as solved.
void TestSymbolSwapEngine::unbalancedTargetNeverFalselyClaimsSuccess()
{
    const QVector<PairSet> balancedSets = allBalancedPairSets();
    const PairSet fixedStart = balancedSets.first();

    const QVector<SymbolTypes> symbols = {SymbolTypes::Dreieck, SymbolTypes::Viereck,
                                           SymbolTypes::Kreis};
    int tested = 0;

    for (auto a1a : symbols) for (auto a1b : symbols)
    for (auto a2a : symbols) for (auto a2b : symbols)
    for (auto a3a : symbols) for (auto a3b : symbols) {
        PairSet target = {{a1a, a1b}, {a2a, a2b}, {a3a, a3b}};
        if (isBalanced(target))
            continue;

        SymbolSwapEngine engine;
        engine.solve(fixedStart, target);
        ++tested;

        QVERIFY(!engine.isSolved());
    }

    qDebug() << "Exhaustively verified" << tested << "unbalanced targets all correctly report unsolved.";
    QCOMPARE(tested, 729 - 90);
}

QTEST_MAIN(TestSymbolSwapEngine)
#include "tst_symbolswapengine.moc"
