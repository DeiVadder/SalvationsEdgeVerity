#include "calculatesteps.h"

CalculateSteps::CalculateSteps(QObject *parent)
    : QObject{parent}
{}

void CalculateSteps::calculateSteps(SymbolTypes innerStatue1,
                                    SymbolTypes innerStatue2,
                                    SymbolTypes innerStatue3,
                                    SymbolTypes outerStatue1,
                                    SymbolTypes outerStatue2,
                                    SymbolTypes outerStatue3)
{
    qDebug() << innerStatue1 << innerStatue2 << innerStatue3;
    m_swapOperations.clear();
    QVector<QVector<SymbolTypes>> start = {toBaseSymbols(outerStatue1),
                                           toBaseSymbols(outerStatue2),
                                           toBaseSymbols(outerStatue3)};
    QVector<QVector<SymbolTypes>> stop = {fromBaseSymbol(innerStatue1),
                                          fromBaseSymbol(innerStatue2),
                                          fromBaseSymbol(innerStatue3)};

    qDebug() << start;
    qDebug() << stop;

    while (!isFinished(start, stop)) {
        orderSymbolsInPairs(start);
        orderSymbolsInPairs(stop);

        if (!findAndSwap(start, stop))
            break;
    }
    numberOfStepsChanged();
}

CalculateSteps::SymbolTypes CalculateSteps::getInstructionForStep(int step, int statue)
{
    if (step < 0 || step >= numberOfSteps())
        return Undefined;
    auto s1 = m_swapOperations.at(step * 2);
    auto s2 = m_swapOperations.at(step * 2 + 1);
    if (s1.first == statue) {
        return s1.second;
    }
    if (s2.first == statue) {
        return s2.second;
    }
    return Undefined;
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
    m_swapOperations.clear();
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

void CalculateSteps::orderSymbolsInPairs(QVector<QVector<SymbolTypes>> &toOrder)
{
    for (auto &pair : toOrder) {
        if (pair.at(0) > pair.at(1)) {
            std::swap(pair[0], pair[1]);
        }
    }
}

bool CalculateSteps::isFinished(const QVector<QVector<SymbolTypes>> &start,
                                const QVector<QVector<SymbolTypes>> &ziel)
{
    return start == ziel;
}

bool CalculateSteps::findAndSwap(QVector<QVector<SymbolTypes>> &start,
                                 QVector<QVector<SymbolTypes>> &stop)
{
    for (int i = 0; i < start.size(); ++i) {
        if (start.at(i) != stop.at(i)) {
            // A discrepancy between start and ziel found, we need to swap
            auto startPair = start.at(i);
            auto stopPair = stop.at(i);

            bool firstSymbolIsCorrect = stopPair.contains(startPair.at(0));
            // The first symbol is correct, so the second one is wrong
            SymbolTypes wrongSymbol = firstSymbolIsCorrect ? startPair.at(1) : startPair.at(0);

            SymbolTypes targetSymbol;
            if (firstSymbolIsCorrect) {
                targetSymbol = stopPair.at(0) == startPair.at(0) ? stopPair.at(1) : stopPair.at(0);
            } else {
                targetSymbol = stopPair.at(1) == startPair.at(1) ? stopPair.at(0) : stopPair.at(1);
            }

            // Look for a suitable swap partner
            for (int j = 0; j < start.size(); ++j) {
                if (i == j) {
                    //No swapping with oneself
                    continue;
                }

                // Check if the swap is possible
                if (start.at(j) != stop.at(j) && //Symbol is not finsihed
                    start.at(j).contains(targetSymbol)
                    && //Theres at least 1 symbol that can be changed
                    (!stop.at(j).contains(targetSymbol) || start.at(j).count(targetSymbol) == 2))
                //Target symbol is not needed for solution, or there are currently 2 of those
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
