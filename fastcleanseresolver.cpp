#include "fastcleanseresolver.h"

#include <algorithm>

namespace {
using SymbolTypes = FastCleanseResolver::SymbolTypes;

int ownerOf(const QVector<SymbolTypes> &ownSymbols, SymbolTypes symbol)
{
    for (int i = 0; i < ownSymbols.size(); ++i) {
        if (ownSymbols.at(i) == symbol)
            return i;
    }
    return -1;
}

// The player that is neither `self` nor `other` (there are exactly 3
// players). If `other` is invalid or equals `self`, any player other
// than `self` is returned.
int thirdPlayer(int self, int other)
{
    for (int i = 0; i < 3; ++i) {
        if (i != self && i != other)
            return i;
    }
    return -1;
}

bool matchesTarget(QVector<SymbolTypes> room, QVector<SymbolTypes> target)
{
    std::sort(room.begin(), room.end());
    std::sort(target.begin(), target.end());
    return room == target;
}
} // namespace

void FastCleanseResolver::resolve(const QVector<SymbolTypes> &ownSymbols,
                                   const QVector<QVector<SymbolTypes>> &wallPairs)
{
    m_transfers.clear();
    m_roundCount = 0;
    m_solved = false;

    if (ownSymbols.size() != 3 || wallPairs.size() != 3)
        return;
    for (const auto &wall : wallPairs) {
        if (wall.size() != 2)
            return;
    }

    QVector<QVector<SymbolTypes>> rooms = wallPairs;
    QVector<QVector<SymbolTypes>> targets;
    for (const auto &own : ownSymbols)
        targets.append(CalculateSteps::fromBaseSymbol(own));

    const int maxRounds = 6;
    for (int round = 0; round < maxRounds; ++round) {
        QVector<Transfer> roundTransfers;

        for (int i = 0; i < 3; ++i) {
            const QVector<SymbolTypes> &room = rooms.at(i);
            SymbolTypes own = ownSymbols.at(i);

            if (matchesTarget(room, targets.at(i)))
                continue;

            if (room.size() == 2 && room.at(0) == own && room.at(1) == own) {
                // {p,p}: give one p to each of the other two players.
                for (int other = 0; other < 3; ++other) {
                    if (other == i)
                        continue;
                    roundTransfers.append({round, i, other, own});
                }
                continue;
            }

            if (room.size() == 2 && room.at(0) == room.at(1) && room.at(0) != own) {
                // {n,n}, a foreign double: keep one, give the other to
                // the third player (not self, not n's own owner - they
                // don't want a 3rd copy of their own symbol).
                SymbolTypes n = room.at(0);
                int recipient = thirdPlayer(i, ownerOf(ownSymbols, n));
                if (recipient >= 0)
                    roundTransfers.append({round, i, recipient, n});
                continue;
            }

            if (room.size() == 2 && room.contains(own)) {
                // {p,n}: keep n, dunk p to n's owner. This is the one
                // ambiguous case - see class doc comment.
                SymbolTypes n = (room.at(0) == own) ? room.at(1) : room.at(0);
                int recipient = ownerOf(ownSymbols, n);
                if (recipient >= 0)
                    roundTransfers.append({round, i, recipient, own});
                continue;
            }

            if (room.size() >= 3) {
                // Fix-up: excess accumulated from a prior round (either
                // a wrong convention guess above, or simply receiving a
                // legitimate transfer while already at target). Peel off
                // the most-duplicated symbol; repeats over subsequent
                // rounds if more than one excess unit is present.
                SymbolTypes excess = CalculateSteps::Undefined;
                int highestCount = 0;
                for (auto s : room) {
                    int c = room.count(s);
                    if (c > highestCount) {
                        highestCount = c;
                        excess = s;
                    }
                }
                if (highestCount >= 2) {
                    int recipient = thirdPlayer(i, ownerOf(ownSymbols, excess));
                    if (recipient >= 0)
                        roundTransfers.append({round, i, recipient, excess});
                }
                continue;
            }
            // room.size() < 2 or some other unexpected composition:
            // nothing sensible to do this round.
        }

        if (roundTransfers.isEmpty())
            break;

        m_transfers.append(roundTransfers);
        m_roundCount = round + 1;

        for (const auto &t : roundTransfers) {
            rooms[t.fromPlayer].removeOne(t.symbol);
            rooms[t.toPlayer].append(t.symbol);
        }
    }

    m_solved = true;
    for (int i = 0; i < 3; ++i) {
        if (!matchesTarget(rooms.at(i), targets.at(i))) {
            m_solved = false;
            break;
        }
    }
}

void FastCleanseResolver::reset()
{
    m_transfers.clear();
    m_roundCount = 0;
    m_solved = false;
}

FastCleanseResolver::Transfer FastCleanseResolver::transfer(int index) const
{
    return m_transfers.value(index, Transfer{});
}
