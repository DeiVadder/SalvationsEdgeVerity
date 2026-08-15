#include <QtTest>

#include "puzzle/lfgsortresolver.h"

using SymbolTypes = CalculateSteps::SymbolTypes;

namespace {

int countSymbol(const std::initializer_list<SymbolTypes> &values, SymbolTypes symbol)
{
    int count = 0;
    for (auto s : values) {
        if (s == symbol) {
            ++count;
        }
    }
    return count;
}

// Independently replays the resolver's own recorded transfer list against
// a fresh copy of the starting walls - a correctness oracle that does NOT
// rely on LFGSortResolver's own internal isSolved() bookkeeping, so it
// can't share a bug with it.
QVector<QVector<SymbolTypes>> replayTransfers(const QVector<QVector<SymbolTypes>> &initialWalls,
                                               const LFGSortResolver &resolver)
{
    QVector<QVector<SymbolTypes>> rooms = initialWalls;
    for (int i = 0; i < resolver.numberOfTransfers(); ++i) {
        auto t = resolver.transfer(i);
        rooms[t.fromPlayer].removeOne(t.symbol);
        rooms[t.toPlayer].append(t.symbol);
    }
    return rooms;
}

bool matchesSelfPair(QVector<SymbolTypes> room, SymbolTypes own)
{
    return room.size() == 2 && room.at(0) == own && room.at(1) == own;
}

} // namespace

class TestLFGSortResolver : public QObject
{
    Q_OBJECT
private:
    LFGSortResolver *m_resolver = nullptr;
    QVector<SymbolTypes> m_ownSymbols;

private slots:
    void init();
    void cleanup();

    void resolveConvergesForEveryBalancedWall();
    void alreadyDoubledPlayerNeedsNoTransfer();
    void zeroOwnCopiesPlayerSendsBoth();
    void realReportedScenarioMatchesDirectHandoffs();
    void resetClearsTransfers();
};

void TestLFGSortResolver::init()
{
    m_resolver = new LFGSortResolver();
    m_ownSymbols = {CalculateSteps::Dreieck, CalculateSteps::Viereck, CalculateSteps::Kreis};
}

void TestLFGSortResolver::cleanup()
{
    delete m_resolver;
    m_resolver = nullptr;
}

// Exhaustively covers all 90 balanced wall combinations (see
// tst_calculateinsidesteps.cpp's checkIsValidWallExhaustive for the same
// 3^6=729 generation, 90 of which balance) - for every one, every player
// must end up holding exactly {own,own} after replaying the resolver's
// own transfer list, and every transfer must be a DIRECT hand-off (the
// symbol given away always equals the recipient's own symbol - no
// relaying through a 3rd party).
void TestLFGSortResolver::resolveConvergesForEveryBalancedWall()
{
    const QVector<SymbolTypes> symbols = {CalculateSteps::Dreieck, CalculateSteps::Viereck,
                                           CalculateSteps::Kreis};
    int balancedCount = 0;

    for (auto w1a : symbols) { for (auto w1b : symbols) {
    for (auto w2a : symbols) { for (auto w2b : symbols) {
    for (auto w3a : symbols) { for (auto w3b : symbols) {
        bool balanced = countSymbol({w1a, w1b, w2a, w2b, w3a, w3b}, CalculateSteps::Dreieck) == 2
            && countSymbol({w1a, w1b, w2a, w2b, w3a, w3b}, CalculateSteps::Viereck) == 2
            && countSymbol({w1a, w1b, w2a, w2b, w3a, w3b}, CalculateSteps::Kreis) == 2;
        if (!balanced) {
            continue;
        }
        ++balancedCount;

        QVector<QVector<SymbolTypes>> walls = {{w1a, w1b}, {w2a, w2b}, {w3a, w3b}};
        m_resolver->resolve(m_ownSymbols, walls);

        QVERIFY(m_resolver->isSolved());

        for (int i = 0; i < m_resolver->numberOfTransfers(); ++i) {
            auto t = m_resolver->transfer(i);
            QVERIFY(t.fromPlayer >= 0 && t.fromPlayer < 3);
            QVERIFY(t.toPlayer >= 0 && t.toPlayer < 3);
            QVERIFY(t.fromPlayer != t.toPlayer);
            // Direct hand-off: the symbol always belongs to the recipient.
            QCOMPARE(t.symbol, m_ownSymbols.at(t.toPlayer));
            // The giver never gives away their own symbol.
            QVERIFY(t.symbol != m_ownSymbols.at(t.fromPlayer));
        }

        auto result = replayTransfers(walls, *m_resolver);
        for (int i = 0; i < 3; ++i) {
            QVERIFY(matchesSelfPair(result.at(i), m_ownSymbols.at(i)));
        }
    }
    }
    }
    }
    }
    }

    QCOMPARE(balancedCount, 90);
}

void TestLFGSortResolver::alreadyDoubledPlayerNeedsNoTransfer()
{
    // Player 0 (own Dreieck) already shows {Dreieck,Dreieck} - the other
    // 2 Dreieck copies live entirely with players 1/2 in this balanced
    // configuration, so player 0 sends and receives nothing.
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Dreieck},
                                            {CalculateSteps::Viereck, CalculateSteps::Kreis},
                                            {CalculateSteps::Viereck, CalculateSteps::Kreis}};
    m_resolver->resolve(m_ownSymbols, walls);

    QVERIFY(m_resolver->isSolved());
    for (int i = 0; i < m_resolver->numberOfTransfers(); ++i) {
        auto t = m_resolver->transfer(i);
        QVERIFY(t.fromPlayer != 0);
        QVERIFY(t.toPlayer != 0);
    }

    auto result = replayTransfers(walls, *m_resolver);
    for (int i = 0; i < 3; ++i) {
        QVERIFY(matchesSelfPair(result.at(i), m_ownSymbols.at(i)));
    }
}

void TestLFGSortResolver::zeroOwnCopiesPlayerSendsBoth()
{
    // Player 0 (own Dreieck) holds {Viereck,Kreis} - 0 copies of their own
    // symbol. Both wall slots are foreign, so they send 2 transfers this
    // round (one to each of the other 2 players), never receive a 3rd
    // symbol in return here.
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Viereck, CalculateSteps::Kreis},
                                            {CalculateSteps::Dreieck, CalculateSteps::Viereck},
                                            {CalculateSteps::Dreieck, CalculateSteps::Kreis}};
    m_resolver->resolve(m_ownSymbols, walls);

    QVERIFY(m_resolver->isSolved());

    int player0Gives = 0;
    for (int i = 0; i < m_resolver->numberOfTransfers(); ++i) {
        auto t = m_resolver->transfer(i);
        if (t.fromPlayer == 0) {
            ++player0Gives;
        }
    }
    QCOMPARE(player0Gives, 2);

    auto result = replayTransfers(walls, *m_resolver);
    for (int i = 0; i < 3; ++i) {
        QVERIFY(matchesSelfPair(result.at(i), m_ownSymbols.at(i)));
    }
}

// The exact scenario reported from real gameplay (2026-08-16): LEFT holds
// {own, RIGHT's}, MID holds {own, LEFT's}, RIGHT holds {own, MID's} - a
// 3-cycle. Real report: "LEFT gives Kreis to RIGHT" - confirms each
// player gives directly to the rightful owner, no relay through the 3rd
// player (which a generic pairwise-swap solver produced instead).
void TestLFGSortResolver::realReportedScenarioMatchesDirectHandoffs()
{
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Kreis},
                                            {CalculateSteps::Viereck, CalculateSteps::Dreieck},
                                            {CalculateSteps::Kreis, CalculateSteps::Viereck}};
    m_resolver->resolve(m_ownSymbols, walls);

    QVERIFY(m_resolver->isSolved());
    QCOMPARE(m_resolver->numberOfTransfers(), 3);

    bool leftGivesKreisToRight = false;
    bool midGivesDreieckToLeft = false;
    bool rightGivesViereckToMid = false;
    for (int i = 0; i < m_resolver->numberOfTransfers(); ++i) {
        auto t = m_resolver->transfer(i);
        if (t.fromPlayer == 0 && t.toPlayer == 2 && t.symbol == CalculateSteps::Kreis) {
            leftGivesKreisToRight = true;
        }
        if (t.fromPlayer == 1 && t.toPlayer == 0 && t.symbol == CalculateSteps::Dreieck) {
            midGivesDreieckToLeft = true;
        }
        if (t.fromPlayer == 2 && t.toPlayer == 1 && t.symbol == CalculateSteps::Viereck) {
            rightGivesViereckToMid = true;
        }
    }
    QVERIFY(leftGivesKreisToRight);
    QVERIFY(midGivesDreieckToLeft);
    QVERIFY(rightGivesViereckToMid);
}

void TestLFGSortResolver::resetClearsTransfers()
{
    QVector<QVector<SymbolTypes>> walls = {{CalculateSteps::Dreieck, CalculateSteps::Kreis},
                                            {CalculateSteps::Viereck, CalculateSteps::Dreieck},
                                            {CalculateSteps::Kreis, CalculateSteps::Viereck}};
    m_resolver->resolve(m_ownSymbols, walls);
    QVERIFY(m_resolver->numberOfTransfers() > 0);

    m_resolver->reset();
    QCOMPARE(m_resolver->numberOfTransfers(), 0);
    QVERIFY(!m_resolver->isSolved());
}

QTEST_MAIN(TestLFGSortResolver)
#include "tst_lfgsortresolver.moc"
