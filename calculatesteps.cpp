#include "calculatesteps.h"

#include "symbolswapengine.h"

CalculateSteps::CalculateSteps(QObject *parent)
    : QObject{parent}
    , m_engine{std::make_unique<SymbolSwapEngine>()}
{}

CalculateSteps::~CalculateSteps() = default;

void CalculateSteps::calculateSteps(SymbolTypes innerStatue1,
                                    SymbolTypes innerStatue2,
                                    SymbolTypes innerStatue3,
                                    SymbolTypes outerStatue1,
                                    SymbolTypes outerStatue2,
                                    SymbolTypes outerStatue3)
{
    QVector<QVector<SymbolTypes>> start = {toBaseSymbols(outerStatue1),
                                           toBaseSymbols(outerStatue2),
                                           toBaseSymbols(outerStatue3)};
    QVector<QVector<SymbolTypes>> stop = {fromBaseSymbol(innerStatue1),
                                          fromBaseSymbol(innerStatue2),
                                          fromBaseSymbol(innerStatue3)};

    m_engine->solve(start, stop);
    numberOfStepsChanged();
}

int CalculateSteps::numberOfSteps()
{
    return m_engine->numberOfSteps();
}

CalculateSteps::SymbolTypes CalculateSteps::getInstructionForStep(int step, int statue)
{
    return m_engine->getInstructionForStep(step, statue);
}

bool CalculateSteps::checkIsValid(SymbolTypes innerStatue1,
                                  SymbolTypes innerStatue2,
                                  SymbolTypes innerStatue3,
                                  SymbolTypes outerStatue1,
                                  SymbolTypes outerStatue2,
                                  SymbolTypes outerStatue3)
{
    QVector<QVector<SymbolTypes>> start = {toBaseSymbols(outerStatue1),
                                           toBaseSymbols(outerStatue2),
                                           toBaseSymbols(outerStatue3)};
    QVector<QVector<SymbolTypes>> stop = {fromBaseSymbol(innerStatue1),
                                          fromBaseSymbol(innerStatue2),
                                          fromBaseSymbol(innerStatue3)};

    QVector<SymbolTypes> failure{Undefined, Undefined};

    if (start.contains(failure) || stop.contains(failure)) {
        qDebug() << Q_FUNC_INFO << "Contains failure" << start << stop;
        return false;
    }

    int cntKreis{0}, cntDreieck{0}, cntViereck{0};
    for (auto pair : start) {
        if (pair.contains(Kreis))
            cntKreis += pair.count(Kreis);
        if (pair.contains(Dreieck))
            cntDreieck += pair.count(Dreieck);
        if (pair.contains(Viereck))
            cntViereck += pair.count(Viereck);
    }
    if (cntKreis != cntDreieck || cntKreis != cntViereck) {
        qDebug() << Q_FUNC_INFO << "incorrect start" << cntKreis << cntDreieck << cntViereck;
        return false;
    }

    for (auto pair : stop) {
        if (pair.contains(Kreis))
            cntKreis++;
        if (pair.contains(Dreieck))
            cntDreieck++;
        if (pair.contains(Viereck))
            cntViereck++;
    }
    if (cntKreis != cntDreieck || cntKreis != cntViereck) {
        qDebug() << Q_FUNC_INFO << "incorrect stop";
        return false;
    }

    return true;
}

void CalculateSteps::reset()
{
    m_engine->reset();
    numberOfStepsChanged();
}

QVector<CalculateSteps::SymbolTypes> CalculateSteps::toBaseSymbols(SymbolTypes type)
{
    switch (type) {
    case SymbolTypes::Kegel:
        return {SymbolTypes::Dreieck, SymbolTypes::Kreis};

    case SymbolTypes::Zylinder:
        return {SymbolTypes::Kreis, SymbolTypes::Viereck};

    case SymbolTypes::Prisma:
        return {SymbolTypes::Viereck, SymbolTypes::Dreieck};

    case SymbolTypes::Wuerfel:
        return {SymbolTypes::Viereck, SymbolTypes::Viereck};

    case SymbolTypes::Pyramide:
        return {SymbolTypes::Dreieck, SymbolTypes::Dreieck};
    case SymbolTypes::Kugel:
        return {SymbolTypes::Kreis, SymbolTypes::Kreis};

    case SymbolTypes::Undefined:
    case SymbolTypes::Dreieck:
    case SymbolTypes::Viereck:
    case SymbolTypes::Kreis:
    default:
        return {SymbolTypes::Undefined, SymbolTypes::Undefined};
    }
}

QVector<CalculateSteps::SymbolTypes> CalculateSteps::fromBaseSymbol(SymbolTypes type)
{
    switch (type) {
    case Dreieck:
        return {Viereck, Kreis};
    case Viereck:
        return {Dreieck, Kreis};
    case Kreis:
        return {Dreieck, Viereck};
    default:
        return {Undefined, Undefined};
    }
}

CalculateSteps::SymbolTypes CalculateSteps::pairToShape(SymbolTypes a, SymbolTypes b)
{
    if (a > b)
        std::swap(a, b);

    if (a == Dreieck && b == Dreieck)
        return Pyramide;
    if (a == Dreieck && b == Viereck)
        return Prisma;
    if (a == Dreieck && b == Kreis)
        return Kegel;
    if (a == Viereck && b == Viereck)
        return Wuerfel;
    if (a == Viereck && b == Kreis)
        return Zylinder;
    if (a == Kreis && b == Kreis)
        return Kugel;

    return Undefined;
}
