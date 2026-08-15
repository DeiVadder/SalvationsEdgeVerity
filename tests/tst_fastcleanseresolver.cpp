#include <QtTest>
#include "fastcleanseresolver.h"

using SymbolTypes = CalculateSteps::SymbolTypes;

class TestFastCleanseResolver : public QObject
{
    Q_OBJECT
private:
    FastCleanseResolver *m_resolver = nullptr;
    // Fixed own-symbol assignment used by every test: player0=Dreieck,
    // player1=Viereck, player2=Kreis. target0={Viereck,Kreis},
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

QTEST_MAIN(TestFastCleanseResolver)
#include "tst_fastcleanseresolver.moc"
