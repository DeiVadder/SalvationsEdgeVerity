#ifndef CALCULATEINSIDESTEPS_H
#define CALCULATEINSIDESTEPS_H

#include <memory>

#include <QObject>
#include <QVector>

#include "calculatesteps.h"

class SymbolSwapEngine;

// Solves the inside/solo-room puzzle: 3 teleported players each start
// holding one 2D symbol (pairwise distinct) and, via the cleanse+distribute
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
// cleanse/distribute step sequence are derived from research + user
// description, not verified against actual gameplay. Sanity-check the
// computed target pairs / a sample step sequence before relying on this
// for a real raid (see Plan 2's "Offene Frage 1").
class CalculateInsideSteps : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int numberOfSteps READ numberOfSteps NOTIFY numberOfStepsChanged FINAL)
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
    Q_INVOKABLE bool isSolved() const;
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
    Q_INVOKABLE SymbolTypes finalShapeForPlayer(int player) const;

public slots:
    void reset();

signals:
    void numberOfStepsChanged();

private:
    std::unique_ptr<SymbolSwapEngine> m_engine;
    QVector<SymbolTypes> m_targetShapePerPlayer;
};

#endif // CALCULATEINSIDESTEPS_H
