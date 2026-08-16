#include <algorithm>

#include <QtTest>

#include "puzzle/lfgdistributeresolver.h"

using SymbolTypes = CalculateSteps::SymbolTypes;

namespace {

// Independently replays the resolver's own recorded transfer list against
// a fresh {own,own} starting state per player - a correctness oracle that
// does NOT rely on LFGDistributeResolver's own isSolved() bookkeeping.
QVector<QVector<SymbolTypes>> replayTransfers(const QVector<SymbolTypes> &ownSymbols,
                                               const LFGDistributeResolver &resolver)
{
    QVector<QVector<SymbolTypes>> rooms;
    for (auto own : ownSymbols) {
        rooms.append({own, own});
    }
    for (int i = 0; i < resolver.numberOfTransfers(); ++i) {
        auto t = resolver.transfer(i);
        rooms[t.fromPlayer].removeOne(t.symbol);
        rooms[t.toPlayer].append(t.symbol);
    }
    return rooms;
}

bool matchesOtherTwo(QVector<SymbolTypes> room, const QVector<SymbolTypes> &ownSymbols, int player)
{
    if (room.size() != 2) {
        return false;
    }
    QVector<SymbolTypes> expected;
    for (int i = 0; i < ownSymbols.size(); ++i) {
        if (i != player) {
            expected.append(ownSymbols.at(i));
        }
    }
    std::sort(room.begin(), room.end());
    std::sort(expected.begin(), expected.end());
    return room == expected;
}

} // namespace

class TestLFGDistributeResolver : public QObject
{
    Q_OBJECT
private:
    LFGDistributeResolver *m_resolver = nullptr;
    QVector<SymbolTypes> m_ownSymbols;

private slots:
    void init();
    void cleanup();

    void resolveIsAlwaysDirectAndConverges();
    void realReportedScenarioMatchesDirectHandoffs();
    void resetClearsTransfers();
};

void TestLFGDistributeResolver::init()
{
    m_resolver = new LFGDistributeResolver();
    m_ownSymbols = {CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis};
}

void TestLFGDistributeResolver::cleanup()
{
    delete m_resolver;
    m_resolver = nullptr;
}

// Own symbols are always pairwise distinct (checkIsValid()'s invariant),
// but the actual 3 symbols/positions can permute - covers all 6
// permutations of {Dreieck,Viereck,Kreis} across LEFT/MID/RIGHT.
void TestLFGDistributeResolver::resolveIsAlwaysDirectAndConverges()
{
    QVector<QVector<SymbolTypes>> permutations = {
        {CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis},
        {CalculateSteps::Dreieck, CalculateSteps::Kreis, CalculateSteps::Viereck},
        {CalculateSteps::Viereck, CalculateSteps::Dreieck, CalculateSteps::Kreis},
        {CalculateSteps::Viereck, CalculateSteps::Kreis, CalculateSteps::Dreieck},
        {CalculateSteps::Kreis, CalculateSteps::Dreieck, CalculateSteps::Viereck},
        {CalculateSteps::Kreis, CalculateSteps::Viereck, CalculateSteps::Dreieck},
    };

    for (const auto &own : permutations) {
        m_resolver->resolve(own);

        QVERIFY(m_resolver->isSolved());
        QCOMPARE(m_resolver->numberOfRounds(), 2);
        QCOMPARE(m_resolver->numberOfTransfers(), 6);

        for (int i = 0; i < m_resolver->numberOfTransfers(); ++i) {
            auto t = m_resolver->transfer(i);
            QVERIFY(t.round == 0 || t.round == 1);
            QVERIFY(t.fromPlayer >= 0 && t.fromPlayer < 3);
            QVERIFY(t.toPlayer >= 0 && t.toPlayer < 3);
            QVERIFY(t.fromPlayer != t.toPlayer);
            // Direct hand-off: always the giver's own symbol, and always
            // something the recipient still needs (not their own symbol).
            QCOMPARE(t.symbol, own.at(t.fromPlayer));
            QVERIFY(t.symbol != own.at(t.toPlayer));
        }

        auto result = replayTransfers(own, *m_resolver);
        for (int i = 0; i < 3; ++i) {
            QVERIFY(matchesOtherTwo(result.at(i), own, i));
        }
    }
}

// The exact scenario reported from real gameplay (2026-08-16): LEFT=Dreieck,
// MID=Viereck, RIGHT=Kreis. Real report: "Kreis links abgeben, Kreis mitte
// abgeben" - RIGHT/Kreis gives directly to LEFT in one round and directly
// to MID in the other, never to their own column and never relayed.
void TestLFGDistributeResolver::realReportedScenarioMatchesDirectHandoffs()
{
    m_resolver->resolve(m_ownSymbols); // {Dreieck, Viereck, Kreis} = LEFT, MID, RIGHT

    QVERIFY(m_resolver->isSolved());

    bool rightGivesKreisToLeft = false;
    bool rightGivesKreisToMid = false;
    for (int i = 0; i < m_resolver->numberOfTransfers(); ++i) {
        auto t = m_resolver->transfer(i);
        if (t.fromPlayer == 2 && t.toPlayer == 0 && t.symbol == CalculateSteps::Kreis) {
            rightGivesKreisToLeft = true;
        }
        if (t.fromPlayer == 2 && t.toPlayer == 1 && t.symbol == CalculateSteps::Kreis) {
            rightGivesKreisToMid = true;
        }
    }
    QVERIFY(rightGivesKreisToLeft);
    QVERIFY(rightGivesKreisToMid);
}

void TestLFGDistributeResolver::resetClearsTransfers()
{
    m_resolver->resolve(m_ownSymbols);
    QVERIFY(m_resolver->numberOfTransfers() > 0);

    m_resolver->reset();
    QCOMPARE(m_resolver->numberOfTransfers(), 0);
    QCOMPARE(m_resolver->numberOfRounds(), 0);
    QVERIFY(!m_resolver->isSolved());
}

QTEST_MAIN(TestLFGDistributeResolver)
#include "tst_lfgdistributeresolver.moc"
