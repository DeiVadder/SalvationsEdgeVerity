#include "calculatesteps.h"

#include "symbolswapengine.h"

CalculateSteps::CalculateSteps(QObject *parent)
    : QObject{parent}
    , m_engine{std::make_unique<SymbolSwapEngine>()}
{}

CalculateSteps::~CalculateSteps() = default;

void CalculateSteps::bumpCalculationVersion()
{
    ++m_calculationVersion;
    emit calculationVersionChanged();
    emit numberOfStepsChanged();
}

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

    m_targetShapePerStatue.clear();
    for (const auto &pair : stop) {
        m_targetShapePerStatue.append(pairToShape(pair.at(0), pair.at(1)));
    }

    bumpCalculationVersion();
}

int CalculateSteps::numberOfSteps()
{
    return m_engine->numberOfSteps();
}

CalculateSteps::SymbolTypes CalculateSteps::getInstructionForStep(int step, int statue)
{
    return m_engine->getInstructionForStep(step, statue);
}

CalculateSteps::SymbolTypes CalculateSteps::shapeAfterStep(int step, int statue)
{
    const auto pair = m_engine->stateAfterStep(step, statue);
    if (pair.size() != 2) {
        return Undefined;
    }
    return pairToShape(pair.at(0), pair.at(1));
}

bool CalculateSteps::isSolved() const
{
    return m_engine->isSolved();
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

    int cntKreis{0};
    int cntDreieck{0};
    int cntViereck{0};
    for (const auto &pair : start) {
        if (pair.contains(Kreis)) {
            cntKreis += static_cast<int>(pair.count(Kreis));
        }
        if (pair.contains(Dreieck)) {
            cntDreieck += static_cast<int>(pair.count(Dreieck));
        }
        if (pair.contains(Viereck)) {
            cntViereck += static_cast<int>(pair.count(Viereck));
        }
    }
    if (cntKreis != cntDreieck || cntKreis != cntViereck) {
        qDebug() << Q_FUNC_INFO << "incorrect start" << cntKreis << cntDreieck << cntViereck;
        return false;
    }

    for (const auto &pair : stop) {
        if (pair.contains(Kreis)) {
            cntKreis++;
        }
        if (pair.contains(Dreieck)) {
            cntDreieck++;
        }
        if (pair.contains(Viereck)) {
            cntViereck++;
        }
    }
    if (cntKreis != cntDreieck || cntKreis != cntViereck) {
        qDebug() << Q_FUNC_INFO << "incorrect stop";
        return false;
    }

    return true;
}

void CalculateSteps::calculateStepsChallenge(SymbolTypes outerStatue1,
                                              SymbolTypes outerStatue2,
                                              SymbolTypes outerStatue3,
                                              SymbolTypes challengeTarget1,
                                              SymbolTypes challengeTarget2,
                                              SymbolTypes challengeTarget3)
{
    QVector<QVector<SymbolTypes>> start = {toBaseSymbols(outerStatue1),
                                            toBaseSymbols(outerStatue2),
                                            toBaseSymbols(outerStatue3)};
    QVector<QVector<SymbolTypes>> stop = {toBaseSymbols(challengeTarget1),
                                           toBaseSymbols(challengeTarget2),
                                           toBaseSymbols(challengeTarget3)};

    m_engine->solve(start, stop);

    m_targetShapePerStatue = {challengeTarget1, challengeTarget2, challengeTarget3};

    bumpCalculationVersion();
}

bool CalculateSteps::checkIsValidChallenge(SymbolTypes innerStatue1,
                                            SymbolTypes innerStatue2,
                                            SymbolTypes innerStatue3,
                                            SymbolTypes challengeTarget1,
                                            SymbolTypes challengeTarget2,
                                            SymbolTypes challengeTarget3)
{
    QVector<SymbolTypes> inner = {innerStatue1, innerStatue2, innerStatue3};
    for (auto s : inner) {
        if (s != Dreieck && s != Viereck && s != Kreis) {
            return false;
        }
    }
    if (innerStatue1 == innerStatue2 || innerStatue1 == innerStatue3 || innerStatue2 == innerStatue3) {
        return false;
    }

    QVector<QVector<SymbolTypes>> target = {toBaseSymbols(challengeTarget1),
                                             toBaseSymbols(challengeTarget2),
                                             toBaseSymbols(challengeTarget3)};

    QVector<SymbolTypes> failure{Undefined, Undefined};
    if (target.contains(failure)) {
        return false;
    }

    for (int i = 0; i < 3; ++i) {
        if (target.at(i).contains(inner.at(i))) {
            return false;
        }
    }

    int cntKreis{0};
    int cntDreieck{0};
    int cntViereck{0};
    for (const auto &pair : target) {
        cntKreis += static_cast<int>(pair.count(Kreis));
        cntDreieck += static_cast<int>(pair.count(Dreieck));
        cntViereck += static_cast<int>(pair.count(Viereck));
    }
    return cntKreis == cntDreieck && cntKreis == cntViereck;
}

CalculateSteps::SymbolTypes CalculateSteps::targetShapeForStatue(int statue) const
{
    return m_targetShapePerStatue.value(statue, Undefined);
}

void CalculateSteps::reset()
{
    m_engine->reset();
    m_targetShapePerStatue.clear();
    bumpCalculationVersion();
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
    if (a > b) {
        std::swap(a, b);
    }

    if (a == Dreieck && b == Dreieck) {
        return Pyramide;
    }
    if (a == Dreieck && b == Viereck) {
        return Prisma;
    }
    if (a == Dreieck && b == Kreis) {
        return Kegel;
    }
    if (a == Viereck && b == Viereck) {
        return Wuerfel;
    }
    if (a == Viereck && b == Kreis) {
        return Zylinder;
    }
    if (a == Kreis && b == Kreis) {
        return Kugel;
    }

    return Undefined;
}
