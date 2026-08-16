#ifndef CALCULATEINSIDESTEPS_H
#define CALCULATEINSIDESTEPS_H

#include <memory>

#include <QObject>
#include <QVector>

#include "calculatesteps.h"

class SymbolSwapEngine;
class FastCleanseResolver;
class LFGSortResolver;
class LFGDistributeResolver;

// Inside/solo-room puzzle: 3 players each start with 1 own symbol, sort
// then distribute to reach a target pair to combine into an escape shape.
// Default target = fromBaseSymbol(own) (the 2 other symbols). Challenge
// target = outside team's 3 called shapes instead (can include a pure
// double). UNVERIFIED against real gameplay.
class CalculateInsideSteps : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int numberOfSteps READ numberOfSteps NOTIFY numberOfStepsChanged FINAL)
    // Bumps on every calculate*/reset() call - unlike numberOfSteps, whose
    // VALUE can legitimately repeat across calls, so QML can't rely on it
    // alone to know a recalculation happened.
    Q_PROPERTY(int calculationVersion READ calculationVersion NOTIFY calculationVersionChanged FINAL)
public:
    using SymbolTypes = CalculateSteps::SymbolTypes;

    explicit CalculateInsideSteps(QObject *parent = nullptr);
    ~CalculateInsideSteps() override;

    // Precondition: callers must call checkIsValid() with the same arguments first.
    Q_INVOKABLE void calculateSteps(SymbolTypes player1Symbol,
                                     SymbolTypes player2Symbol,
                                     SymbolTypes player3Symbol);

    // Target = the outside caller's 3 shapes instead of the default split.
    // Precondition: checkIsValidChallenge() with the same arguments first.
    Q_INVOKABLE void calculateStepsChallenge(SymbolTypes player1Symbol,
                                              SymbolTypes player2Symbol,
                                              SymbolTypes player3Symbol,
                                              SymbolTypes outerTarget1,
                                              SymbolTypes outerTarget2,
                                              SymbolTypes outerTarget3);

    Q_INVOKABLE int numberOfSteps();
    Q_INVOKABLE SymbolTypes getInstructionForStep(int step, int player);
    Q_INVOKABLE [[nodiscard]] bool isSolved() const;
    Q_INVOKABLE bool checkIsValid(SymbolTypes player1Symbol,
                                   SymbolTypes player2Symbol,
                                   SymbolTypes player3Symbol);
    // Player symbols valid + distinct, targets defined + balanced (each
    // base symbol twice), and no target contains its own player's symbol
    // (mechanically impossible - see calculatesteps.h's checkIsValidChallenge).
    Q_INVOKABLE bool checkIsValidChallenge(SymbolTypes player1Symbol,
                                            SymbolTypes player2Symbol,
                                            SymbolTypes player3Symbol,
                                            SymbolTypes outerTarget1,
                                            SymbolTypes outerTarget2,
                                            SymbolTypes outerTarget3);
    Q_INVOKABLE [[nodiscard]] SymbolTypes finalShapeForPlayer(int player) const;
    [[nodiscard]] int calculationVersion() const { return m_calculationVersion; }

    // Distribute, default target only: direct-hand-off closed form (see
    // lfgdistributeresolver.h), computed alongside calculateSteps() -
    // NOT the generic engine above. Not populated by Challenge (pure-double
    // targets have no closed form).
    Q_INVOKABLE [[nodiscard]] int numberOfDistributeRounds() const;
    Q_INVOKABLE [[nodiscard]] int numberOfDistributeTransfers() const;
    Q_INVOKABLE [[nodiscard]] int distributeTransferRound(int index) const;
    Q_INVOKABLE [[nodiscard]] int distributeTransferFrom(int index) const;
    Q_INVOKABLE [[nodiscard]] int distributeTransferTo(int index) const;
    Q_INVOKABLE [[nodiscard]] SymbolTypes distributeTransferSymbol(int index) const;
    Q_INVOKABLE [[nodiscard]] bool isDistributeSolved() const;

    // LFG/Fast: take the actual observed wall content per player (2
    // symbols each, not assumed to include their own symbol). Not called
    // by the current UI (see InsideRoomPanel.qml's mySortTransfers, which
    // computes the app user's own sort steps locally instead) - kept as
    // tested API for a possible multi-player-visible view later.
    Q_INVOKABLE bool checkIsValidWall(SymbolTypes player1Symbol,
                                       SymbolTypes player2Symbol,
                                       SymbolTypes player3Symbol,
                                       SymbolTypes wall1a, SymbolTypes wall1b,
                                       SymbolTypes wall2a, SymbolTypes wall2b,
                                       SymbolTypes wall3a, SymbolTypes wall3b);

    // Sort (every foreign wall symbol directly to its owner, via
    // LFGSortResolver - not the generic engine, see lfgsortresolver.h)
    // then distribute (same target formula as calculateSteps()).
    Q_INVOKABLE void calculateStepsLFG(SymbolTypes player1Symbol,
                                        SymbolTypes player2Symbol,
                                        SymbolTypes player3Symbol,
                                        SymbolTypes wall1a, SymbolTypes wall1b,
                                        SymbolTypes wall2a, SymbolTypes wall2b,
                                        SymbolTypes wall3a, SymbolTypes wall3b);
    Q_INVOKABLE [[nodiscard]] int numberOfSortTransfers() const;
    Q_INVOKABLE [[nodiscard]] int sortTransferFrom(int index) const;
    Q_INVOKABLE [[nodiscard]] int sortTransferTo(int index) const;
    Q_INVOKABLE [[nodiscard]] SymbolTypes sortTransferSymbol(int index) const;
    Q_INVOKABLE [[nodiscard]] bool isSortSolved() const;

    // Sort unchanged; distribute targets the outside caller's 3 shapes via
    // the generic engine (no closed form for pure-double targets).
    // Precondition: checkIsValidWallChallenge().
    Q_INVOKABLE bool checkIsValidWallChallenge(SymbolTypes player1Symbol,
                                                SymbolTypes player2Symbol,
                                                SymbolTypes player3Symbol,
                                                SymbolTypes wall1a, SymbolTypes wall1b,
                                                SymbolTypes wall2a, SymbolTypes wall2b,
                                                SymbolTypes wall3a, SymbolTypes wall3b,
                                                SymbolTypes outerTarget1,
                                                SymbolTypes outerTarget2,
                                                SymbolTypes outerTarget3);
    Q_INVOKABLE void calculateStepsLFGChallenge(SymbolTypes player1Symbol,
                                                 SymbolTypes player2Symbol,
                                                 SymbolTypes player3Symbol,
                                                 SymbolTypes wall1a, SymbolTypes wall1b,
                                                 SymbolTypes wall2a, SymbolTypes wall2b,
                                                 SymbolTypes wall3a, SymbolTypes wall3b,
                                                 SymbolTypes outerTarget1,
                                                 SymbolTypes outerTarget2,
                                                 SymbolTypes outerTarget3);

    // Local per-player decision table, no sync callout - see
    // FastCleanseResolver. Only valid for the default target (its 5 cases
    // were verified for "two other symbols" only) - not offered in
    // Challenge mode. Not called by the current UI (no reachable button).
    Q_INVOKABLE void calculateStepsFast(SymbolTypes player1Symbol,
                                         SymbolTypes player2Symbol,
                                         SymbolTypes player3Symbol,
                                         SymbolTypes wall1a, SymbolTypes wall1b,
                                         SymbolTypes wall2a, SymbolTypes wall2b,
                                         SymbolTypes wall3a, SymbolTypes wall3b);
    Q_INVOKABLE [[nodiscard]] int numberOfFastRounds() const;
    Q_INVOKABLE [[nodiscard]] int numberOfFastTransfers() const;
    Q_INVOKABLE [[nodiscard]] int fastTransferRound(int index) const;
    Q_INVOKABLE [[nodiscard]] int fastTransferFrom(int index) const;
    Q_INVOKABLE [[nodiscard]] int fastTransferTo(int index) const;
    Q_INVOKABLE [[nodiscard]] SymbolTypes fastTransferSymbol(int index) const;
    Q_INVOKABLE [[nodiscard]] bool isFastSolved() const;

public slots:
    void reset();

signals:
    void numberOfStepsChanged();
    void calculationVersionChanged();

private:
    void bumpCalculationVersion();

    std::unique_ptr<SymbolSwapEngine> m_engine;
    std::unique_ptr<LFGSortResolver> m_sortResolver;
    std::unique_ptr<LFGDistributeResolver> m_distributeResolver;
    std::unique_ptr<FastCleanseResolver> m_fastResolver;
    QVector<SymbolTypes> m_targetShapePerPlayer;
    int m_calculationVersion = 0;
};

#endif // CALCULATEINSIDESTEPS_H
