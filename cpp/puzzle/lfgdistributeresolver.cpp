#include "lfgdistributeresolver.h"

void LFGDistributeResolver::resolve(const QVector<SymbolTypes> &ownSymbols)
{
    reset();
    if (ownSymbols.size() != 3) {
        return;
    }

    for (int i = 0; i < 3; ++i) {
        const int next = (i + 1) % 3;
        const int prev = (i + 2) % 3;
        m_transfers.append({0, i, next, ownSymbols.at(i)});
        m_transfers.append({1, i, prev, ownSymbols.at(i)});
    }
    m_solved = true;
}

void LFGDistributeResolver::reset()
{
    m_transfers.clear();
    m_solved = false;
}

LFGDistributeResolver::Transfer LFGDistributeResolver::transfer(int index) const
{
    return m_transfers.value(index, Transfer{});
}
