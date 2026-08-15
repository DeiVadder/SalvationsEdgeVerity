#include "calculateinsidesteps.h"

#include "symbolswapengine.h"

namespace {
bool isBase2dSymbol(CalculateInsideSteps::SymbolTypes type)
{
    return type == CalculateSteps::Dreieck || type == CalculateSteps::Viereck
           || type == CalculateSteps::Kreis;
}
} // namespace

CalculateInsideSteps::CalculateInsideSteps(QObject *parent)
    : QObject{parent}
    , m_engine{std::make_unique<SymbolSwapEngine>()}
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
    for (const auto &pair : target)
        m_targetShapePerPlayer.append(CalculateSteps::pairToShape(pair.at(0), pair.at(1)));

    numberOfStepsChanged();
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

    numberOfStepsChanged();
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
        || !isBase2dSymbol(player3Symbol))
        return false;

    // The game guarantees each solo player's own statue shows a different
    // symbol from the other two.
    if (player1Symbol == player2Symbol || player1Symbol == player3Symbol
        || player2Symbol == player3Symbol)
        return false;

    return true;
}

bool CalculateInsideSteps::checkIsValidChallenge(SymbolTypes player1Symbol,
                                                  SymbolTypes player2Symbol,
                                                  SymbolTypes player3Symbol,
                                                  SymbolTypes outerTarget1,
                                                  SymbolTypes outerTarget2,
                                                  SymbolTypes outerTarget3)
{
    if (!checkIsValid(player1Symbol, player2Symbol, player3Symbol))
        return false;

    QVector<QVector<SymbolTypes>> target = {CalculateSteps::toBaseSymbols(outerTarget1),
                                             CalculateSteps::toBaseSymbols(outerTarget2),
                                             CalculateSteps::toBaseSymbols(outerTarget3)};

    QVector<SymbolTypes> failure{CalculateSteps::Undefined, CalculateSteps::Undefined};
    if (target.contains(failure))
        return false;

    int cntKreis{0}, cntDreieck{0}, cntViereck{0};
    for (const auto &pair : target) {
        cntKreis += pair.count(CalculateSteps::Kreis);
        cntDreieck += pair.count(CalculateSteps::Dreieck);
        cntViereck += pair.count(CalculateSteps::Viereck);
    }
    return cntKreis == cntDreieck && cntKreis == cntViereck;
}

CalculateInsideSteps::SymbolTypes CalculateInsideSteps::finalShapeForPlayer(int player) const
{
    return m_targetShapePerPlayer.value(player, SymbolTypes::Undefined);
}

void CalculateInsideSteps::reset()
{
    m_engine->reset();
    m_targetShapePerPlayer.clear();
    numberOfStepsChanged();
}
