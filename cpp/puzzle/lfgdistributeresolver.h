#pragma once

#include <QVector>

#include "calculatesteps.h"

// Distribute phase (default/LFG target only): once every player has 2x
// their own symbol (after Sort - see LFGSortResolver), each player needs
// to end up with 1 copy of each of the OTHER 2 symbols (their final
// escape-shape pair). The generic SymbolSwapEngine solves this correctly
// but sometimes relays a symbol through the 3rd/uninvolved player instead
// of a direct hand-off, same class of mismatch as the old cleanse phase
// (see LFGSortResolver) - confirmed by a real reported scenario: the Kreis
// owner (RIGHT) was shown giving Kreis to their OWN column, then a step
// with no visible action for them, instead of 2 direct gives.
//
// This closed form needs no solver at all: since ownSymbols are always
// pairwise distinct, giving 1 copy of your own symbol to EACH of the other
// 2 players is always exactly what both of them need (a mathematical
// identity, not a coincidence of any particular input) - so the sequence
// is fixed regardless of which symbols were picked. Split into 2
// synchronized rounds (round 1 = give to the next player in LEFT->MID->
// RIGHT->LEFT rotation, round 2 = give to the previous player in that
// rotation) so nobody ever needs to give both their copies away before
// receiving anything back. Hand-derived and confirmed against the same
// real reported scenario as LFGSortResolver (RIGHT/Kreis gives to LEFT in
// round 1, to MID in round 2 - matches "Kreis links abgeben, Kreis mitte
// abgeben", 2026-08-16).
//
// Only valid for the default target formula (fromBaseSymbol(own)) -
// Challenge mode's outside-called targets can include pure-double shapes
// that break the "always the other 2 symbols" assumption, so Challenge
// keeps using the generic engine.
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
