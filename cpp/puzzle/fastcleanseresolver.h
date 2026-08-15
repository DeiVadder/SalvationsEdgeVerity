#ifndef FASTCLEANSERESOLVER_H
#define FASTCLEANSERESOLVER_H

#include <QVector>

#include "calculatesteps.h"

// Local, callout-free "Fast" cleanse protocol for the inside/solo-room
// puzzle's default (non-challenge) target: each player decides their own
// give-away action purely from their own currently-held 2 (or, mid-fixup,
// 3) symbols - no coordination or "which neighbor symbol is on my wall"
// formula needed, since real gameplay guides indicate a solo room's
// starting content is NOT a fixed function of the 3 players' own statue
// symbols (a room can start already-solved, or with a doubled symbol).
//
// UNVERIFIED AGAINST REAL GAMEPLAY. This models a protocol designed via
// AI-assisted analysis (not confirmed against an actual raid run) - see
// Plan 2's "Offene Frage 1" and calculateinsidesteps.h. Sanity-check
// against a live encounter before trusting these instructions in a real
// raid.
//
// One wall type - holding your own symbol plus exactly one of the two
// symbols you need - is provably ambiguous from local information alone:
// two different true room layouts are consistent with the same
// observation, and each calls for a different action. The resolver picks
// a fixed convention for that case (dunk your own symbol to whoever owns
// the other symbol you're holding) and self-corrects via a fix-up round
// if that guess produced an excess (3-of-a-kind) in some room.
class FastCleanseResolver
{
public:
    using SymbolTypes = CalculateSteps::SymbolTypes;

    struct Transfer {
        int round = -1;
        int fromPlayer = -1;
        int toPlayer = -1;
        SymbolTypes symbol = CalculateSteps::Undefined;
    };

    // ownSymbols has exactly 3 entries (LEFT/MID/RIGHT). wallPairs has
    // exactly 3 entries, each itself exactly 2 symbols - what that
    // player's room currently holds. Each player's target is implicitly
    // fromBaseSymbol(ownSymbols[i]) (the two OTHER symbols) - the
    // default, non-challenge formula only; this protocol has not been
    // derived for challenge/pure-double targets.
    void resolve(const QVector<SymbolTypes> &ownSymbols,
                 const QVector<QVector<SymbolTypes>> &wallPairs);

    void reset();
    [[nodiscard]] bool isSolved() const { return m_solved; }
    [[nodiscard]] int numberOfRounds() const { return m_roundCount; }
    [[nodiscard]] int numberOfTransfers() const { return static_cast<int>(m_transfers.size()); }
    [[nodiscard]] Transfer transfer(int index) const;

private:
    QVector<Transfer> m_transfers;
    int m_roundCount = 0;
    bool m_solved = false;
};

#endif // FASTCLEANSERESOLVER_H
