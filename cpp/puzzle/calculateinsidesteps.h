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

// Solves the inside/solo-room puzzle: 3 teleported players each start
// holding one 2D symbol (pairwise distinct) and, via the sort+distribute
// mechanic, end up holding a target pair to combine into their escape
// shape. Modeled as start={{p,p}} (each player's own symbol as a trivial
// self-pair) -> target, reusing the same SymbolSwapEngine the outside
// puzzle (CalculateSteps) uses.
//
// Two target modes:
// - Default (calculateSteps()): target=fromBaseSymbol(p) - each player ends
//   up with the two OTHER symbols. True whenever the outside team isn't
//   running a challenge that needs a "pure" double escape shape
//   (Wuerfel/Pyramide/Kugel) from the inside room.
// - Challenge (calculateStepsChallenge()): target is dictated directly by
//   the outside caller's 3 chosen outside 3D shapes (relayed verbally,
//   since inside players can't see the outside statues) via
//   toBaseSymbols(). If one of those shapes is a pure double, the matching
//   player receives both copies of that base symbol instead of the default
//   one-each split - the generic swap engine handles this without special
//   casing, since it's simply a different (still balanced) target set.
//
// NOTE: the target formula and the claim that this reproduces a real
// sort/distribute step sequence are derived from research + user
// description, not verified against actual gameplay. Sanity-check the
// computed target pairs / a sample step sequence before relying on this
// for a real raid (see Plan 2's "Offene Frage 1").
class CalculateInsideSteps : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int numberOfSteps READ numberOfSteps NOTIFY numberOfStepsChanged FINAL)
    // Bumped on every calculateSteps*/reset() call, regardless of which
    // internal engine it touches. numberOfSteps alone isn't a reliable
    // QML re-evaluation trigger for calculateStepsFast()/calculateStepsLFG()
    // cleanse phase, since those don't necessarily change numberOfSteps's
    // own value (e.g. Fast never touches the distribute engine at all) -
    // QML only re-fires dependent bindings when a watched property's VALUE
    // changes, not merely when its NOTIFY signal fires. This counter always
    // changes, so it's always safe to depend on for that trick.
    Q_PROPERTY(int calculationVersion READ calculationVersion NOTIFY calculationVersionChanged FINAL)
public:
    using SymbolTypes = CalculateSteps::SymbolTypes;

    explicit CalculateInsideSteps(QObject *parent = nullptr);
    ~CalculateInsideSteps() override;

    // Precondition: callers must call checkIsValid() with the same arguments first.
    Q_INVOKABLE void calculateSteps(SymbolTypes player1Symbol,
                                     SymbolTypes player2Symbol,
                                     SymbolTypes player3Symbol);

    // Challenge-mode variant: each player's final held pair is dictated by
    // the outside caller's 3 outside 3D shapes (outerTargetN) instead of
    // always being "the two other symbols". Needed whenever one of those
    // shapes is a pure double (Wuerfel/Pyramide/Kugel) - the corresponding
    // player must then receive both copies of one base symbol instead of
    // the default one-each split. Precondition: callers must call
    // checkIsValidChallenge() with the same arguments first.
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
    // Validates a calculateStepsChallenge() call: player symbols must pass
    // the normal checkIsValid(), and the 3 outer target shapes must be
    // defined and balanced (each base symbol appears exactly twice across
    // them - the same invariant CalculateSteps::checkIsValid() enforces for
    // the outside puzzle, since these ARE that puzzle's 3 statue targets).
    Q_INVOKABLE bool checkIsValidChallenge(SymbolTypes player1Symbol,
                                            SymbolTypes player2Symbol,
                                            SymbolTypes player3Symbol,
                                            SymbolTypes outerTarget1,
                                            SymbolTypes outerTarget2,
                                            SymbolTypes outerTarget3);
    Q_INVOKABLE [[nodiscard]] SymbolTypes finalShapeForPlayer(int player) const;
    [[nodiscard]] int calculationVersion() const { return m_calculationVersion; }

    // Distribute, default target only: direct-hand-off closed form (see
    // lfgdistributeresolver.h) computed alongside calculateSteps() and
    // calculateStepsLFG() - NOT the generic engine's numberOfSteps()/
    // getInstructionForStep() above, which still relays through the 3rd
    // player. Not populated by the Challenge-mode variants (pure-double
    // targets have no closed form - see lfgdistributeresolver.h).
    Q_INVOKABLE [[nodiscard]] int numberOfDistributeRounds() const;
    Q_INVOKABLE [[nodiscard]] int numberOfDistributeTransfers() const;
    Q_INVOKABLE [[nodiscard]] int distributeTransferRound(int index) const;
    Q_INVOKABLE [[nodiscard]] int distributeTransferFrom(int index) const;
    Q_INVOKABLE [[nodiscard]] int distributeTransferTo(int index) const;
    Q_INVOKABLE [[nodiscard]] SymbolTypes distributeTransferSymbol(int index) const;
    Q_INVOKABLE [[nodiscard]] bool isDistributeSolved() const;

    // LFG / Fast: both take the actual observed wall content per player
    // (2 symbols each, NOT assumed to include the player's own symbol -
    // see fastcleanseresolver.h for why no fixed derivation from the 3
    // own-symbols exists) instead of assuming an already-cleansed
    // self-pair start. UNVERIFIED AGAINST REAL GAMEPLAY - see
    // fastcleanseresolver.h and Plan 2's "Offene Frage 1".
    Q_INVOKABLE bool checkIsValidWall(SymbolTypes player1Symbol,
                                       SymbolTypes player2Symbol,
                                       SymbolTypes player3Symbol,
                                       SymbolTypes wall1a, SymbolTypes wall1b,
                                       SymbolTypes wall2a, SymbolTypes wall2b,
                                       SymbolTypes wall3a, SymbolTypes wall3b);

    // LFG: two sequential phases - sort (every foreign wall symbol goes
    // directly to its owner, via LFGSortResolver - NOT the generic
    // pairwise-swap SymbolSwapEngine, see lfgsortresolver.h for why) then
    // distribute (self-pair -> fromBaseSymbol(p), same as the default
    // calculateSteps()). Real-world equivalent: an explicit "wait until
    // everyone has sorted" callout sits between the two phases.
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

    // LFG + Challenge: sort phase unchanged (every foreign wall symbol to
    // its owner), but the distribute phase targets the outside caller's 3
    // shapes instead of the default fromBaseSymbol(p) - still needs the
    // generic SymbolSwapEngine there (already proven exhaustively, see
    // tst_symbolswapengine.cpp, to converge for ANY balanced target, not
    // just the default one - unlike sort/default-distribute, a pure-double
    // challenge target has no simple direct-transfer closed form).
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

    // Fast: local per-player decision table, no phases, no synchronization
    // callout - see FastCleanseResolver. Deliberately NOT offered in
    // Challenge mode: the decision table's 5 cases were derived and
    // exhaustively verified only for the default "two other symbols"
    // target shape. Generic Challenge targets (in particular pure-double
    // ones) can produce a room/target combination none of the 5 cases
    // matches (e.g. wall={B,C}, own=A, target={A,A}) - the player would
    // simply never act, silently stuck forever. Fixing that needs a real
    // redesign + fresh exhaustive verification, not a parameter add.
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
