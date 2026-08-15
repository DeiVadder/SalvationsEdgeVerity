#include <QtTest>

#include <algorithm>

#include "puzzle/fastcleanseresolver.h"

using SymbolTypes = CalculateSteps::SymbolTypes;

namespace {

QString symbolName(SymbolTypes s)
{
    switch (s) {
    case CalculateSteps::Dreieck: return "Dreieck";
    case CalculateSteps::Viereck: return "Viereck";
    case CalculateSteps::Kreis: return "Kreis";
    default: return "?";
    }
}

QString describeSymbols(const QVector<SymbolTypes> &v)
{
    QStringList parts;
    for (auto s : v)
        parts << symbolName(s);
    return parts.join(",");
}

QString describeWalls(const QVector<QVector<SymbolTypes>> &walls)
{
    QStringList parts;
    for (const auto &w : walls)
        parts << QString("{%1}").arg(describeSymbols(w));
    return parts.join(" ");
}

bool matchesSorted(QVector<SymbolTypes> a, QVector<SymbolTypes> b)
{
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

// Independently replays the resolver's own recorded transfer list against
// a fresh copy of the starting walls - a correctness oracle that does NOT
// rely on FastCleanseResolver's own internal isSolved() bookkeeping, so it
// can't share a bug with it.
QVector<QVector<SymbolTypes>> replayTransfers(const QVector<QVector<SymbolTypes>> &initialWalls,
                                               const FastCleanseResolver &resolver)
{
    QVector<QVector<SymbolTypes>> rooms = initialWalls;
    for (int i = 0; i < resolver.numberOfTransfers(); ++i) {
        auto t = resolver.transfer(i);
        rooms[t.fromPlayer].removeOne(t.symbol);
        rooms[t.toPlayer].append(t.symbol);
    }
    return rooms;
}

int countSymbol(const QVector<QVector<SymbolTypes>> &walls, SymbolTypes symbol)
{
    int count = 0;
    for (const auto &w : walls)
        count += w.count(symbol);
    return count;
}

bool isBalanced(const QVector<QVector<SymbolTypes>> &walls)
{
    return countSymbol(walls, CalculateSteps::Dreieck) == 2
        && countSymbol(walls, CalculateSteps::Viereck) == 2
        && countSymbol(walls, CalculateSteps::Kreis) == 2;
}

} // namespace

class TestFastCleanseResolver : public QObject
{
    Q_OBJECT
private:
    FastCleanseResolver *m_resolver = nullptr;
    // Fixed own-symbol assignment used by the hand-picked scenario tests:
    // player0=Dreieck, player1=Viereck, player2=Kreis. target0={Viereck,Kreis},
    // target1={Dreieck,Kreis}, target2={Dreieck,Viereck}.
    const QVector<SymbolTypes> m_own{CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                      CalculateSteps::Kreis};

private slots:
    void init();
    void cleanup();

    void alreadyCorrectConvergesImmediately();
    void allOwnDoubledConvergesInOneRound();
    void ambiguousGuessCorrectConvergesInOneRound();
    void ambiguousGuessWrongSelfCorrectsInTwoRounds();
    void resetClearsState();
    void invalidInputSizeDoesNotCrash();

    // Exhaustive coverage, per the "as many combinations as possible,
    // including expected failures" request.
    void allBalancedConfigurationsConvergeToCorrectTarget();
    void allUnbalancedConfigurationsNeverFalselyClaimSuccess();
    void malformedStructuralInputsFailGracefully();
};

void TestFastCleanseResolver::init()
{
    m_resolver = new FastCleanseResolver();
}

void TestFastCleanseResolver::cleanup()
{
    delete m_resolver;
    m_resolver = nullptr;
}

void TestFastCleanseResolver::alreadyCorrectConvergesImmediately()
{
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Viereck, CalculateSteps::Kreis},
                                            {CalculateSteps::Dreieck, CalculateSteps::Kreis},
                                            {CalculateSteps::Dreieck, CalculateSteps::Viereck}};

    m_resolver->resolve(m_own, walls);

    QVERIFY(m_resolver->isSolved());
    QCOMPARE(m_resolver->numberOfRounds(), 0);
    QCOMPARE(m_resolver->numberOfTransfers(), 0);
}

// Every wall shows the player's own symbol doubled - each gives one copy
// to each of the other two, converging in exactly 1 round (6 transfers).
void TestFastCleanseResolver::allOwnDoubledConvergesInOneRound()
{
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Dreieck},
                                            {CalculateSteps::Viereck, CalculateSteps::Viereck},
                                            {CalculateSteps::Kreis, CalculateSteps::Kreis}};

    m_resolver->resolve(m_own, walls);

    QVERIFY(m_resolver->isSolved());
    QCOMPARE(m_resolver->numberOfRounds(), 1);
    QCOMPARE(m_resolver->numberOfTransfers(), 6);
}

// wall0={Dreieck,Viereck} (own+other, the ambiguous case) - true hidden
// layout here is such that the fixed convention (dunk own symbol to the
// other-held symbol's owner) happens to be correct. Converges in 1 round.
void TestFastCleanseResolver::ambiguousGuessCorrectConvergesInOneRound()
{
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Viereck},
                                            {CalculateSteps::Kreis, CalculateSteps::Kreis},
                                            {CalculateSteps::Dreieck, CalculateSteps::Viereck}};

    m_resolver->resolve(m_own, walls);

    QVERIFY(m_resolver->isSolved());
    QCOMPARE(m_resolver->numberOfRounds(), 1);
    QCOMPARE(m_resolver->numberOfTransfers(), 2);
}

// Both player0 and player1 hit the ambiguous "own+other" case and their
// fixed-convention guesses are BOTH wrong for this particular hidden
// layout (proven possible - the same local observation is consistent
// with multiple true layouts needing different actions). This produces
// a round-1 overshoot (player0 and player1 both end up holding 3
// symbols), which the fix-up rule must resolve in round 2 without any
// extra communication.
void TestFastCleanseResolver::ambiguousGuessWrongSelfCorrectsInTwoRounds()
{
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Viereck},
                                            {CalculateSteps::Viereck, CalculateSteps::Dreieck},
                                            {CalculateSteps::Kreis, CalculateSteps::Kreis}};

    m_resolver->resolve(m_own, walls);

    QVERIFY(m_resolver->isSolved());
    QCOMPARE(m_resolver->numberOfRounds(), 2);
}

void TestFastCleanseResolver::resetClearsState()
{
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Dreieck},
                                            {CalculateSteps::Viereck, CalculateSteps::Viereck},
                                            {CalculateSteps::Kreis, CalculateSteps::Kreis}};
    m_resolver->resolve(m_own, walls);
    QVERIFY(m_resolver->numberOfTransfers() > 0);

    m_resolver->reset();

    QVERIFY(!m_resolver->isSolved());
    QCOMPARE(m_resolver->numberOfRounds(), 0);
    QCOMPARE(m_resolver->numberOfTransfers(), 0);
}

void TestFastCleanseResolver::invalidInputSizeDoesNotCrash()
{
    QVector<QVector<SymbolTypes>> tooFewWalls = {{CalculateSteps::Dreieck, CalculateSteps::Dreieck},
                                                  {CalculateSteps::Viereck, CalculateSteps::Viereck}};

    m_resolver->resolve(m_own, tooFewWalls);

    QVERIFY(!m_resolver->isSolved());
    QCOMPARE(m_resolver->numberOfRounds(), 0);
}

// Exhaustively enumerates every (own-symbol permutation) x (wall
// configuration) pair where the wall is balanced (each of the 3 base
// symbols appears exactly twice across the 3 walls combined - the
// invariant checkIsValidWall() enforces at the CalculateInsideSteps
// layer). That's 6 permutations x 90 balanced raw wall sequences = 540
// configurations. For every single one: the resolver must converge
// (isSolved()==true), AND an INDEPENDENT replay of its own recorded
// transfer list (not touching its internal isSolved() bookkeeping) must
// land every player exactly on fromBaseSymbol(ownSymbol).
void TestFastCleanseResolver::allBalancedConfigurationsConvergeToCorrectTarget()
{
    const QVector<SymbolTypes> symbols = {CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                           CalculateSteps::Kreis};
    QVector<SymbolTypes> ownPermutation = symbols;
    std::sort(ownPermutation.begin(), ownPermutation.end());

    int testedCount = 0;
    int maxRoundsSeen = 0;

    do {
        QVector<QVector<SymbolTypes>> targets;
        for (auto own : ownPermutation)
            targets.append(CalculateSteps::fromBaseSymbol(own));

        for (auto a1a : symbols) for (auto a1b : symbols)
        for (auto a2a : symbols) for (auto a2b : symbols)
        for (auto a3a : symbols) for (auto a3b : symbols) {
            QVector<QVector<SymbolTypes>> walls = {{a1a, a1b}, {a2a, a2b}, {a3a, a3b}};
            if (!isBalanced(walls))
                continue;

            FastCleanseResolver resolver;
            resolver.resolve(ownPermutation, walls);
            ++testedCount;

            QVERIFY2(resolver.isSolved(),
                     qPrintable(QString("Did not converge: own=[%1] walls=%2")
                                    .arg(describeSymbols(ownPermutation), describeWalls(walls))));
            QVERIFY(resolver.numberOfRounds() <= 6);

            auto finalRooms = replayTransfers(walls, resolver);
            for (int p = 0; p < 3; ++p) {
                QVERIFY2(matchesSorted(finalRooms.at(p), targets.at(p)),
                         qPrintable(QString("Wrong final state for player %1: own=[%2] walls=%3")
                                        .arg(p)
                                        .arg(describeSymbols(ownPermutation), describeWalls(walls))));
            }

            maxRoundsSeen = qMax(maxRoundsSeen, resolver.numberOfRounds());
        }
    } while (std::next_permutation(ownPermutation.begin(), ownPermutation.end()));

    qDebug() << "Exhaustively verified" << testedCount
             << "balanced (own-symbols x wall) configurations, max rounds seen:" << maxRoundsSeen;
    QCOMPARE(testedCount, 6 * 90);
}

// The complementary "expect fail" sweep: every UNBALANCED wall
// configuration (639 of the 729 raw combinations, for a fixed own-symbol
// assignment - conservation of symbol count makes this independent of
// which permutation is used) must NEVER be reported as solved. Transfers
// only move existing symbols between players, so an unbalanced start can
// never reach a balanced target - a resolver claiming success here would
// be silently handing out wrong instructions.
void TestFastCleanseResolver::allUnbalancedConfigurationsNeverFalselyClaimSuccess()
{
    const QVector<SymbolTypes> symbols = {CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                           CalculateSteps::Kreis};

    int testedCount = 0;

    for (auto a1a : symbols) for (auto a1b : symbols)
    for (auto a2a : symbols) for (auto a2b : symbols)
    for (auto a3a : symbols) for (auto a3b : symbols) {
        QVector<QVector<SymbolTypes>> walls = {{a1a, a1b}, {a2a, a2b}, {a3a, a3b}};
        if (isBalanced(walls))
            continue;

        FastCleanseResolver resolver;
        resolver.resolve(m_own, walls);
        ++testedCount;

        QVERIFY2(!resolver.isSolved(),
                 qPrintable(QString("Falsely claimed success for unbalanced walls=%1")
                                .arg(describeWalls(walls))));
        QVERIFY(resolver.numberOfRounds() <= 6);
    }

    qDebug() << "Exhaustively verified" << testedCount
             << "unbalanced configurations all correctly report unsolved.";
    QCOMPARE(testedCount, 729 - 90);
}

void TestFastCleanseResolver::malformedStructuralInputsFailGracefully()
{
    // Wrong number of own symbols (2 instead of 3).
    {
        QVector<SymbolTypes> tooFewOwn = {CalculateSteps::Dreieck, CalculateSteps::Viereck};
        QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Dreieck},
                                                {CalculateSteps::Viereck, CalculateSteps::Viereck},
                                                {CalculateSteps::Kreis, CalculateSteps::Kreis}};
        FastCleanseResolver r;
        r.resolve(tooFewOwn, walls);
        QVERIFY(!r.isSolved());
        QCOMPARE(r.numberOfRounds(), 0);
    }
    // A wall pair with only 1 symbol instead of 2.
    {
        QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck},
                                                {CalculateSteps::Viereck, CalculateSteps::Viereck},
                                                {CalculateSteps::Kreis, CalculateSteps::Kreis}};
        FastCleanseResolver r;
        r.resolve(m_own, walls);
        QVERIFY(!r.isSolved());
        QCOMPARE(r.numberOfRounds(), 0);
    }
    // A wall pair with 3 symbols instead of 2, from the very start (not
    // just mid-simulation excess).
    {
        QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Dreieck,
                                                  CalculateSteps::Dreieck},
                                                {CalculateSteps::Viereck, CalculateSteps::Viereck},
                                                {CalculateSteps::Kreis, CalculateSteps::Kreis}};
        FastCleanseResolver r;
        r.resolve(m_own, walls);
        QVERIFY(!r.isSolved());
        QCOMPARE(r.numberOfRounds(), 0);
    }
    // Undefined/out-of-alphabet symbols in the wall.
    {
        QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Undefined, CalculateSteps::Dreieck},
                                                {CalculateSteps::Viereck, CalculateSteps::Viereck},
                                                {CalculateSteps::Kreis, CalculateSteps::Kreis}};
        FastCleanseResolver r;
        r.resolve(m_own, walls);
        // Not structurally rejected (sizes are all 2) but can never
        // balance to 2/2/2 with an Undefined occupying a slot, so must
        // not converge.
        QVERIFY(!r.isSolved());
        QVERIFY(r.numberOfRounds() <= 6);
    }
}

QTEST_MAIN(TestFastCleanseResolver)
#include "tst_fastcleanseresolver.moc"
