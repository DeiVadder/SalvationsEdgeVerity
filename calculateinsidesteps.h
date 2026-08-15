#ifndef CALCULATEINSIDESTEPS_H
#define CALCULATEINSIDESTEPS_H

#include <memory>

#include <QObject>
#include <QVector>

#include "calculatesteps.h"

class SymbolSwapEngine;

// Solves the inside/solo-room puzzle: 3 teleported players each start
// holding one 2D symbol (pairwise distinct) and, via the cleanse+distribute
// mechanic, end up holding the two OTHER symbols to combine into their
// escape shape. Modeled as start={{p,p}} (each player's own symbol as a
// trivial self-pair) -> target=fromBaseSymbol(p), reusing the same
// SymbolSwapEngine the outside puzzle (CalculateSteps) uses.
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

    Q_INVOKABLE int numberOfSteps();
    Q_INVOKABLE SymbolTypes getInstructionForStep(int step, int player);
    Q_INVOKABLE bool isSolved() const;
    Q_INVOKABLE bool checkIsValid(SymbolTypes player1Symbol,
                                   SymbolTypes player2Symbol,
                                   SymbolTypes player3Symbol);
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
