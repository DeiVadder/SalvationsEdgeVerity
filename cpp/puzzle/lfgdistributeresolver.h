#pragma once

#include <QVector>

#include "calculatesteps.h"

// Distribute phase, default target only: after Sort, each player has 2x
// their own symbol and needs 1 copy of each OTHER symbol. Needs no solver:
// giving 1 copy of your own symbol to each of the other 2 players is
// always exactly what they need (pairwise-distinct symbols guarantee
// this), so the sequence is fixed regardless of input. Split into 2 rounds
// (give to next / give to prev in LEFT->MID->RIGHT rotation) so nobody
// gives both copies away before receiving anything back. Confirmed
// against a real reported scenario (2026-08-16).
//
// Default target formula only - Challenge's outside-called targets can be
// pure doubles, which break the "always the other 2 symbols" assumption,
// so Challenge keeps using the generic engine.
class LFGDistributeResolver
{
public:
    using SymbolTypes = CalculateSteps::SymbolTypes;

    struct Transfer
    {
        int round = -1;
        int fromPlayer = -1;
        int toPlayer = -1;
        SymbolTypes symbol = CalculateSteps::Undefined;
    };

    void resolve(const QVector<SymbolTypes> &ownSymbols);
    void reset();

    [[nodiscard]] bool isSolved() const { return m_solved; }
    [[nodiscard]] int numberOfRounds() const { return m_solved ? 2 : 0; }
    [[nodiscard]] int numberOfTransfers() const { return static_cast<int>(m_transfers.size()); }
    [[nodiscard]] Transfer transfer(int index) const;

private:
    QVector<Transfer> m_transfers;
    bool m_solved = false;
};
