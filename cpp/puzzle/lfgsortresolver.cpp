#include "lfgsortresolver.h"

void LFGSortResolver::resolve(const QVector<SymbolTypes> &ownSymbols,
                               const QVector<QVector<SymbolTypes>> &wallPairs)
{
    m_transfers.clear();
    m_solved = false;

    if (ownSymbols.size() != 3 || wallPairs.size() != 3) {
        return;
    }

    for (int i = 0; i < 3; ++i) {
        const auto own = ownSymbols.at(i);
        const auto &wall = wallPairs.at(i);
        for (auto symbol : wall) {
            if (symbol == own) {
                continue;
            }
            for (int owner = 0; owner < 3; ++owner) {
                if (owner != i && ownSymbols.at(owner) == symbol) {
                    m_transfers.append({i, owner, symbol});
                    break;
                }
            }
        }
    }

    m_solved = true;
}

void LFGSortResolver::reset()
{
    m_transfers.clear();
    m_solved = false;
}

LFGSortResolver::Transfer LFGSortResolver::transfer(int index) const
{
    return m_transfers.value(index, Transfer{});
}
