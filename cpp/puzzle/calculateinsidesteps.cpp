#include "calculateinsidesteps.h"

#include <algorithm>
#include <array>

#include "fastcleanseresolver.h"
#include "lfgsortresolver.h"
#include "symbolswapengine.h"

namespace {
bool isBase2dSymbol(CalculateInsideSteps::SymbolTypes type)
{
    return type == CalculateSteps::Dreieck || type == CalculateSteps::Viereck
           || type == CalculateSteps::Kreis;
}
} // namespace

void CalculateInsideSteps::bumpCalculationVersion()
{
    ++m_calculationVersion;
    emit calculationVersionChanged();
    emit numberOfStepsChanged();
}

CalculateInsideSteps::CalculateInsideSteps(QObject *parent)
    : QObject{parent}
    , m_engine{std::make_unique<SymbolSwapEngine>()}
    , m_sortResolver{std::make_unique<LFGSortResolver>()}
    , m_fastResolver{std::make_unique<FastCleanseResolver>()}
{}

CalculateInsideSteps::~CalculateInsideSteps() = default;

void CalculateInsideSteps::calculateSteps(SymbolTypes player1Symbol,
                                           SymbolTypes player2Symbol,
                                           SymbolTypes player3Symbol)
{
    QVector<QVector<SymbolTypes>> start = {{player1Symbol, player1Symbol},
                                            {player2Symbol, player2Symbol},
                                            {player3Symbol, player3Symbol}};
    QVector<QVector<SymbolTypes>> target = {CalculateSteps::fromBaseSymbol(player1Symbol),
                                             CalculateSteps::fromBaseSymbol(player2Symbol),
                                             CalculateSteps::fromBaseSymbol(player3Symbol)};

    m_engine->solve(start, target);

    m_targetShapePerPlayer.clear();
    for (const auto &pair : target) {
        m_targetShapePerPlayer.append(CalculateSteps::pairToShape(pair.at(0), pair.at(1)));
    }

    bumpCalculationVersion();
}

void CalculateInsideSteps::calculateStepsChallenge(SymbolTypes player1Symbol,
                                                    SymbolTypes player2Symbol,
                                                    SymbolTypes player3Symbol,
                                                    SymbolTypes outerTarget1,
                                                    SymbolTypes outerTarget2,
                                                    SymbolTypes outerTarget3)
{
    QVector<QVector<SymbolTypes>> start = {{player1Symbol, player1Symbol},
                                            {player2Symbol, player2Symbol},
                                            {player3Symbol, player3Symbol}};
    QVector<QVector<SymbolTypes>> target = {CalculateSteps::toBaseSymbols(outerTarget1),
                                             CalculateSteps::toBaseSymbols(outerTarget2),
                                             CalculateSteps::toBaseSymbols(outerTarget3)};

    m_engine->solve(start, target);

    m_targetShapePerPlayer = {outerTarget1, outerTarget2, outerTarget3};

    bumpCalculationVersion();
}

int CalculateInsideSteps::numberOfSteps()
{
    return m_engine->numberOfSteps();
}

CalculateInsideSteps::SymbolTypes CalculateInsideSteps::getInstructionForStep(int step, int player)
{
    return m_engine->getInstructionForStep(step, player);
}

bool CalculateInsideSteps::isSolved() const
{
    return m_engine->isSolved();
}

bool CalculateInsideSteps::checkIsValid(SymbolTypes player1Symbol,
                                         SymbolTypes player2Symbol,
                                         SymbolTypes player3Symbol)
{
    if (!isBase2dSymbol(player1Symbol) || !isBase2dSymbol(player2Symbol)
        || !isBase2dSymbol(player3Symbol)) {
        return false;
    }

    // The game guarantees each solo player's own statue shows a different
    // symbol from the other two.
    if (player1Symbol == player2Symbol || player1Symbol == player3Symbol
        || player2Symbol == player3Symbol) {
        return false;
    }

    return true;
}

bool CalculateInsideSteps::checkIsValidChallenge(SymbolTypes player1Symbol,
                                                  SymbolTypes player2Symbol,
                                                  SymbolTypes player3Symbol,
                                                  SymbolTypes outerTarget1,
                                                  SymbolTypes outerTarget2,
                                                  SymbolTypes outerTarget3)
{
    if (!checkIsValid(player1Symbol, player2Symbol, player3Symbol)) {
        return false;
    }

    QVector<QVector<SymbolTypes>> target = {CalculateSteps::toBaseSymbols(outerTarget1),
                                             CalculateSteps::toBaseSymbols(outerTarget2),
                                             CalculateSteps::toBaseSymbols(outerTarget3)};

    QVector<SymbolTypes> failure{CalculateSteps::Undefined, CalculateSteps::Undefined};
    if (target.contains(failure)) {
        return false;
    }

    int cntKreis{0};
    int cntDreieck{0};
    int cntViereck{0};
    for (const auto &pair : target) {
        cntKreis += static_cast<int>(pair.count(CalculateSteps::Kreis));
        cntDreieck += static_cast<int>(pair.count(CalculateSteps::Dreieck));
        cntViereck += static_cast<int>(pair.count(CalculateSteps::Viereck));
    }
    return cntKreis == cntDreieck && cntKreis == cntViereck;
}

CalculateInsideSteps::SymbolTypes CalculateInsideSteps::finalShapeForPlayer(int player) const
{
    return m_targetShapePerPlayer.value(player, SymbolTypes::Undefined);
}

bool CalculateInsideSteps::checkIsValidWall(SymbolTypes player1Symbol,
                                             SymbolTypes player2Symbol,
                                             SymbolTypes player3Symbol,
                                             SymbolTypes wall1a, SymbolTypes wall1b,
                                             SymbolTypes wall2a, SymbolTypes wall2b,
                                             SymbolTypes wall3a, SymbolTypes wall3b)
{
    if (!checkIsValid(player1Symbol, player2Symbol, player3Symbol)) {
        return false;
    }

    const std::array walls{wall1a, wall1b, wall2a, wall2b, wall3a, wall3b};
    if (!std::all_of(walls.begin(), walls.end(), isBase2dSymbol)) {
        return false;
    }

    int cntKreis{0};
    int cntDreieck{0};
    int cntViereck{0};
    for (auto s : walls) {
        if (s == CalculateSteps::Kreis) {
            ++cntKreis;
        } else if (s == CalculateSteps::Dreieck) {
            ++cntDreieck;
        } else if (s == CalculateSteps::Viereck) {
            ++cntViereck;
        }
    }
    return cntKreis == 2 && cntDreieck == 2 && cntViereck == 2;
}

void CalculateInsideSteps::calculateStepsLFG(SymbolTypes player1Symbol,
                                              SymbolTypes player2Symbol,
                                              SymbolTypes player3Symbol,
                                              SymbolTypes wall1a, SymbolTypes wall1b,
                                              SymbolTypes wall2a, SymbolTypes wall2b,
                                              SymbolTypes wall3a, SymbolTypes wall3b)
{
    QVector<SymbolTypes> ownSymbols = {player1Symbol, player2Symbol, player3Symbol};
    QVector<QVector<SymbolTypes>> wallPairs = {{wall1a, wall1b}, {wall2a, wall2b}, {wall3a, wall3b}};
    QVector<QVector<SymbolTypes>> selfPairs = {{player1Symbol, player1Symbol},
                                                {player2Symbol, player2Symbol},
                                                {player3Symbol, player3Symbol}};
    QVector<QVector<SymbolTypes>> target = {CalculateSteps::fromBaseSymbol(player1Symbol),
                                             CalculateSteps::fromBaseSymbol(player2Symbol),
                                             CalculateSteps::fromBaseSymbol(player3Symbol)};

    m_sortResolver->resolve(ownSymbols, wallPairs);
    m_engine->solve(selfPairs, target);

    m_targetShapePerPlayer.clear();
    for (const auto &pair : target) {
        m_targetShapePerPlayer.append(CalculateSteps::pairToShape(pair.at(0), pair.at(1)));
    }

    bumpCalculationVersion();
}

bool CalculateInsideSteps::checkIsValidWallChallenge(SymbolTypes player1Symbol,
                                                      SymbolTypes player2Symbol,
                                                      SymbolTypes player3Symbol,
                                                      SymbolTypes wall1a, SymbolTypes wall1b,
                                                      SymbolTypes wall2a, SymbolTypes wall2b,
                                                      SymbolTypes wall3a, SymbolTypes wall3b,
                                                      SymbolTypes outerTarget1,
                                                      SymbolTypes outerTarget2,
                                                      SymbolTypes outerTarget3)
{
    if (!checkIsValidWall(player1Symbol, player2Symbol, player3Symbol,
                           wall1a, wall1b, wall2a, wall2b, wall3a, wall3b)) {
        return false;
    }

    QVector<QVector<SymbolTypes>> target = {CalculateSteps::toBaseSymbols(outerTarget1),
                                             CalculateSteps::toBaseSymbols(outerTarget2),
                                             CalculateSteps::toBaseSymbols(outerTarget3)};

    QVector<SymbolTypes> failure{CalculateSteps::Undefined, CalculateSteps::Undefined};
    if (target.contains(failure)) {
        return false;
    }

    int cntKreis{0};
    int cntDreieck{0};
    int cntViereck{0};
    for (const auto &pair : target) {
        cntKreis += static_cast<int>(pair.count(CalculateSteps::Kreis));
        cntDreieck += static_cast<int>(pair.count(CalculateSteps::Dreieck));
        cntViereck += static_cast<int>(pair.count(CalculateSteps::Viereck));
    }
    return cntKreis == cntDreieck && cntKreis == cntViereck;
}

void CalculateInsideSteps::calculateStepsLFGChallenge(SymbolTypes player1Symbol,
                                                       SymbolTypes player2Symbol,
                                                       SymbolTypes player3Symbol,
                                                       SymbolTypes wall1a, SymbolTypes wall1b,
                                                       SymbolTypes wall2a, SymbolTypes wall2b,
                                                       SymbolTypes wall3a, SymbolTypes wall3b,
                                                       SymbolTypes outerTarget1,
                                                       SymbolTypes outerTarget2,
                                                       SymbolTypes outerTarget3)
{
    QVector<SymbolTypes> ownSymbols = {player1Symbol, player2Symbol, player3Symbol};
    QVector<QVector<SymbolTypes>> wallPairs = {{wall1a, wall1b}, {wall2a, wall2b}, {wall3a, wall3b}};
    QVector<QVector<SymbolTypes>> selfPairs = {{player1Symbol, player1Symbol},
                                                {player2Symbol, player2Symbol},
                                                {player3Symbol, player3Symbol}};
    QVector<QVector<SymbolTypes>> target = {CalculateSteps::toBaseSymbols(outerTarget1),
                                             CalculateSteps::toBaseSymbols(outerTarget2),
                                             CalculateSteps::toBaseSymbols(outerTarget3)};

    m_sortResolver->resolve(ownSymbols, wallPairs);
    m_engine->solve(selfPairs, target);

    m_targetShapePerPlayer = {outerTarget1, outerTarget2, outerTarget3};

    bumpCalculationVersion();
}

int CalculateInsideSteps::numberOfSortTransfers() const
{
    return m_sortResolver->numberOfTransfers();
}

int CalculateInsideSteps::sortTransferFrom(int index) const
{
    return m_sortResolver->transfer(index).fromPlayer;
}

int CalculateInsideSteps::sortTransferTo(int index) const
{
    return m_sortResolver->transfer(index).toPlayer;
}

CalculateInsideSteps::SymbolTypes CalculateInsideSteps::sortTransferSymbol(int index) const
{
    return m_sortResolver->transfer(index).symbol;
}

bool CalculateInsideSteps::isSortSolved() const
{
    return m_sortResolver->isSolved();
}

void CalculateInsideSteps::calculateStepsFast(SymbolTypes player1Symbol,
                                               SymbolTypes player2Symbol,
                                               SymbolTypes player3Symbol,
                                               SymbolTypes wall1a, SymbolTypes wall1b,
                                               SymbolTypes wall2a, SymbolTypes wall2b,
                                               SymbolTypes wall3a, SymbolTypes wall3b)
{
    QVector<SymbolTypes> ownSymbols = {player1Symbol, player2Symbol, player3Symbol};
    QVector<QVector<SymbolTypes>> wallPairs = {{wall1a, wall1b}, {wall2a, wall2b}, {wall3a, wall3b}};

    m_fastResolver->resolve(ownSymbols, wallPairs);

    m_targetShapePerPlayer.clear();
    for (auto own : ownSymbols) {
        auto pair = CalculateSteps::fromBaseSymbol(own);
        m_targetShapePerPlayer.append(CalculateSteps::pairToShape(pair.at(0), pair.at(1)));
    }

    bumpCalculationVersion();
}

int CalculateInsideSteps::numberOfFastRounds() const
{
    return m_fastResolver->numberOfRounds();
}

int CalculateInsideSteps::numberOfFastTransfers() const
{
    return m_fastResolver->numberOfTransfers();
}

int CalculateInsideSteps::fastTransferRound(int index) const
{
    return m_fastResolver->transfer(index).round;
}

int CalculateInsideSteps::fastTransferFrom(int index) const
{
    return m_fastResolver->transfer(index).fromPlayer;
}

int CalculateInsideSteps::fastTransferTo(int index) const
{
    return m_fastResolver->transfer(index).toPlayer;
}

CalculateInsideSteps::SymbolTypes CalculateInsideSteps::fastTransferSymbol(int index) const
{
    return m_fastResolver->transfer(index).symbol;
}

bool CalculateInsideSteps::isFastSolved() const
{
    return m_fastResolver->isSolved();
}

void CalculateInsideSteps::reset()
{
    m_engine->reset();
    m_sortResolver->reset();
    m_fastResolver->reset();
    m_targetShapePerPlayer.clear();
    bumpCalculationVersion();
}
