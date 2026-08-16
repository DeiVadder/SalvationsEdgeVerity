#ifndef LFGSORTRESOLVER_H
#define LFGSORTRESOLVER_H

#include <QVector>

#include "calculatesteps.h"

// Deterministic "give your foreign wall symbol directly to its owner"
// protocol for the inside puzzle's LFG sort phase. Doesn't reuse
// SymbolSwapEngine's pairwise-swap model - a solo room's wall isn't a
// fixed-size pair mid-round (0-6 symbols transiently possible), and a
// generic pairwise-swap search can relay a symbol through an uninvolved
// 3rd player instead of a direct hand-off. Confirmed against a real
// reported sort sequence (2026-08-16); the "0 copies of own symbol" case
// (2 transfers from one player) is inferred, not independently confirmed.
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
