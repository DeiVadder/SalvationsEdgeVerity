#ifndef LFGSORTRESOLVER_H
#define LFGSORTRESOLVER_H

#include <QVector>

#include "calculatesteps.h"

// Deterministic "give your foreign wall symbol directly to its owner"
// protocol for the inside/solo-room puzzle's LFG method - what the app
// used to call the "cleanse" phase, renamed "sort" since ending up
// holding 2 copies of your own symbol is just a byproduct shared by both
// the Sort and Fast approaches, not the point of this phase itself (see
// calculateinsidesteps.h).
//
// Unlike the outside puzzle, a solo room's wall isn't a fixed-size pair
// at every intermediate moment - a player can transiently hold anywhere
// from 0 to 6 symbols while gives are in flight (confirmed 2026-08-16).
// So this does NOT reuse SymbolSwapEngine's pairwise-swap model at all:
// every foreign symbol on a wall goes directly, in one synchronized
// round, to whichever of the other 2 players actually owns it - no
// relaying through a 3rd party, which a generic pairwise-swap search can
// end up doing even though it still reaches the right final state.
//
// Confirmed against a real reported sort sequence (own+1 foreign wall,
// 2026-08-16). The "wall holds 0 copies of own symbol" case (2 transfers
// from the same player) is inferred from the same conversation but not
// independently confirmed - see Plan 2's "Offene Frage 1".
class LFGSortResolver
{
public:
    using SymbolTypes = CalculateSteps::SymbolTypes;

    struct Transfer {
        int fromPlayer = -1;
        int toPlayer = -1;
        SymbolTypes symbol = CalculateSteps::Undefined;
    };

    // ownSymbols has exactly 3 entries (LEFT/MID/RIGHT), pairwise
    // distinct. wallPairs has exactly 3 entries, each itself exactly 2
    // (base 2D) symbols - what that player's room currently holds. Every
    // base symbol must appear exactly twice total across all 6 wall
    // slots (checkIsValidWall's invariant) - not re-validated here.
    void resolve(const QVector<SymbolTypes> &ownSymbols,
                 const QVector<QVector<SymbolTypes>> &wallPairs);
    void reset();

    [[nodiscard]] bool isSolved() const { return m_solved; }
    [[nodiscard]] int numberOfTransfers() const { return static_cast<int>(m_transfers.size()); }
    [[nodiscard]] Transfer transfer(int index) const;

private:
    QVector<Transfer> m_transfers;
    bool m_solved = false;
};

#endif // LFGSORTRESOLVER_H
