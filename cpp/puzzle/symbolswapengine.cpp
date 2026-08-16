#include "symbolswapengine.h"

void SymbolSwapEngine::solve(PairSet start, PairSet target)
{
    m_swapOperations.clear();
    m_stateAfterStep.clear();
    m_solved = false;

    while (!isFinished(start, target)) {
        orderPairs(start);
        orderPairs(target);

        if (!findAndSwap(start, target)) {
            break;
        }
        m_stateAfterStep.append(start);
    }

    orderPairs(start);
    orderPairs(target);
    m_solved = isFinished(start, target);
}

void SymbolSwapEngine::reset()
{
    m_swapOperations.clear();
    m_stateAfterStep.clear();
    m_solved = false;
}

SymbolSwapEngine::SymbolPair SymbolSwapEngine::stateAfterStep(int step, int node) const
{
    if (step < 0 || step >= m_stateAfterStep.size() || node < 0 || node >= m_stateAfterStep.at(step).size()) {
        return {SymbolTypes::Undefined, SymbolTypes::Undefined};
    }
    return m_stateAfterStep.at(step).at(node);
}

SymbolSwapEngine::SymbolTypes SymbolSwapEngine::getInstructionForStep(int step, int node) const
{
    if (step < 0 || step >= numberOfSteps()) {
        return SymbolTypes::Undefined;
    }
    const auto stepIndex = static_cast<qsizetype>(step) * 2;
    auto s1 = m_swapOperations.at(stepIndex);
    auto s2 = m_swapOperations.at(stepIndex + 1);
    if (s1.first == node) {
        return s1.second;
    }
    if (s2.first == node) {
        return s2.second;
    }
    return SymbolTypes::Undefined;
}

void SymbolSwapEngine::orderPairs(PairSet &pairs)
{
    for (auto &pair : pairs) {
        if (pair.at(0) > pair.at(1)) {
            std::swap(pair[0], pair[1]);
        }
    }
}

bool SymbolSwapEngine::isFinished(const PairSet &a, const PairSet &b)
{
    return a == b;
}

// Core swap search, exhaustively verified against all 8100 balanced
// start/target combinations in tst_symbolswapengine.cpp; not worth the
// regression risk of splitting it up for a complexity metric.
bool SymbolSwapEngine::findAndSwap(PairSet &start, PairSet &target)
{
    for (int i = 0; i < start.size(); ++i) {
        if (start.at(i) != target.at(i)) {
            // A discrepancy between start and target found, we need to swap
            auto startPair = start.at(i);
            const auto &targetPair = target.at(i);

            bool firstSymbolIsCorrect = targetPair.contains(startPair.at(0));
            // The first symbol is correct, so the second one is wrong
            SymbolTypes wrongSymbol = firstSymbolIsCorrect ? startPair.at(1) : startPair.at(0);

            SymbolTypes targetSymbol;
            if (firstSymbolIsCorrect) {
                targetSymbol = targetPair.at(0) == startPair.at(0) ? targetPair.at(1)
                                                                    : targetPair.at(0);
            } else {
                targetSymbol = targetPair.at(1) == startPair.at(1) ? targetPair.at(0)
                                                                    : targetPair.at(1);
            }

            // Look for a suitable swap partner
            for (int j = 0; j < start.size(); ++j) {
                if (i == j) {
                    // No swapping with oneself
                    continue;
                }

                // Check if the swap is possible
                if (start.at(j) != target.at(j) && // Symbol is not finished
                    start.at(j).contains(targetSymbol)
                    && // There's at least 1 symbol that can be changed
                    (!target.at(j).contains(targetSymbol) || start.at(j).count(targetSymbol) == 2))
                // Target symbol is not needed for solution, or there are currently 2 of those
                {
                    // Swap the symbols
                    start[j].removeOne(targetSymbol);
                    start[j].append(wrongSymbol);
                    start[i].removeOne(wrongSymbol);
                    start[i].append(targetSymbol);

                    // Record the swap operations
                    m_swapOperations.append(QPair<int, SymbolTypes>{i, wrongSymbol});
                    m_swapOperations.append(QPair<int, SymbolTypes>{j, targetSymbol});
                    return true;
                }
            }
        }
    }
    return false;
}
