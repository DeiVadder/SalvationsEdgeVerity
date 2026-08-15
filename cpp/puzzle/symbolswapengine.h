#ifndef SYMBOLSWAPENGINE_H
#define SYMBOLSWAPENGINE_H

#include <QPair>
#include <QVector>

#include "calculatesteps.h"

// Generic 3-node pair-swap solver shared by the outside dissection puzzle
// (CalculateSteps) and the inside room puzzle (CalculateInsideSteps). Each
// node holds a mutable pair of symbols; solve() finds a sequence of
// single-symbol swaps between nodes that turns `start` into `target`.
class SymbolSwapEngine
{
public:
    using SymbolTypes = CalculateSteps::SymbolTypes;
    using SymbolPair = QVector<SymbolTypes>;
    using PairSet = QVector<SymbolPair>;

    void solve(PairSet start, PairSet target);
    void reset();

    int numberOfSteps() const { return m_swapOperations.size() / 2; }
    SymbolTypes getInstructionForStep(int step, int node) const;
    bool isSolved() const { return m_solved; }

private:
    static void orderPairs(PairSet &pairs);
    static bool isFinished(const PairSet &a, const PairSet &b);
    bool findAndSwap(PairSet &start, PairSet &target);

    QVector<QPair<int, SymbolTypes>> m_swapOperations;
    bool m_solved = false;
};

#endif // SYMBOLSWAPENGINE_H
