#include "calculatesteps.h"

CalculateSteps::CalculateSteps(QObject *parent)
    : QObject{parent}
{}

void CalculateSteps::calculateSteps(SymbolTypes innerStatue1, SymbolTypes innerStatue2, SymbolTypes innerStatue3, SymbolTypes outerStatue1, SymbolTypes outerStatue2, SymbolTypes outerStatue3)
{
    qDebug() << innerStatue1 << innerStatue2 << innerStatue3;
    m_swapOperations.clear();
    QVector<QVector<SymbolTypes> >start = {toBaseSymbols(outerStatue1), toBaseSymbols(outerStatue2), toBaseSymbols(outerStatue3)};
    QVector<QVector<SymbolTypes> >stop = {fromBaseSymbol(innerStatue1), fromBaseSymbol(innerStatue2), fromBaseSymbol(innerStatue3)};

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
    // QVector< QPair<int,SymbolTypes> >
    if(step >  numberOfSteps())
        return Undefined;
    auto s1 = m_swapOperations.at(step * 2);
    auto s2 = m_swapOperations.at(step * 2 +1);
    if(s1.first == statue) {
        return s1.second;
    }
    if(s2.first == statue) {
        return s2.second;
    }
    return Undefined;
}

bool CalculateSteps::checkIsValid(SymbolTypes innerStatue1, SymbolTypes innerStatue2, SymbolTypes innerStatue3, SymbolTypes outerStatue1, SymbolTypes outerStatue2, SymbolTypes outerStatue3)
{
    QVector<QVector<SymbolTypes> >start = {toBaseSymbols(outerStatue1), toBaseSymbols(outerStatue2), toBaseSymbols(outerStatue3)};
    QVector<QVector<SymbolTypes> >stop = {fromBaseSymbol(innerStatue1), fromBaseSymbol(innerStatue2), fromBaseSymbol(innerStatue3)};

    QVector<SymbolTypes> failure {Undefined, Undefined};

    if(start.contains(failure) || stop.contains(failure)){
        qDebug() << Q_FUNC_INFO << "Contains failure" << start << stop;
        return false;
    }

    int cntKreis{0}, cntDreieck{0}, cntViereck{0};
    for(auto pair : start) {
        if(pair.contains(Kreis))
            cntKreis += pair.count(Kreis);
        if(pair.contains(Dreieck))
            cntDreieck += pair.count(Dreieck);
        if(pair.contains(Viereck))
            cntViereck +=pair.count(Viereck);
    }
    if(cntKreis != cntDreieck || cntKreis != cntViereck){
        qDebug() << Q_FUNC_INFO << "incorrect start" << cntKreis << cntDreieck << cntViereck;
        return false;
    }

    for(auto pair : stop) {
        if(pair.contains(Kreis))
            cntKreis++;
        if(pair.contains(Dreieck))
            cntDreieck++;
        if(pair.contains(Viereck))
            cntViereck++;
    }
    if(cntKreis != cntDreieck || cntKreis != cntViereck){
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

QVector<CalculateSteps::SymbolTypes> CalculateSteps::toBaseSymbols(SymbolTypes type) {
    switch (type) {
    // default:
    case SymbolTypes::Undefined:
    case SymbolTypes::Dreieck:
    case SymbolTypes::Viereck:
    case SymbolTypes::Kreis:
        return {SymbolTypes::Undefined, SymbolTypes::Undefined};

    case SymbolTypes::Kegel:
        return {SymbolTypes::Dreieck, SymbolTypes::Kreis};

    case SymbolTypes::Zylinder:
        return {SymbolTypes::Kreis, SymbolTypes::Viereck};

    case SymbolTypes::Prisma:
        return {SymbolTypes::Viereck, SymbolTypes::Dreieck};

    case SymbolTypes::Würfel:
        return {SymbolTypes::Viereck, SymbolTypes::Viereck};

    case SymbolTypes::Pyramide:
        return {SymbolTypes::Dreieck, SymbolTypes::Dreieck};
    case SymbolTypes::Kugel:
        return {SymbolTypes::Kreis, SymbolTypes::Kreis};
    }
}

QVector<CalculateSteps::SymbolTypes> CalculateSteps::fromBaseSymbol(SymbolTypes type)
{
    switch (type) {
    case Dreieck: return {Viereck, Kreis};
    case Viereck: return {Dreieck, Kreis};
    case Kreis:   return {Dreieck, Viereck};
    default:      return {Undefined, Undefined};
    }
}

void CalculateSteps::orderSymbolsInPairs(QVector<QVector<SymbolTypes> > &toOrder) {
    for (auto &pair : toOrder) {
        if (pair.at(0) > pair.at(1)) {
            std::swap(pair[0], pair[1]);
        }
    }
}

bool CalculateSteps::isFinished(const QVector<QVector<SymbolTypes> > &start, const QVector<QVector<SymbolTypes> > &ziel) {
    return start == ziel;
}

bool CalculateSteps::findAndSwap(QVector<QVector<SymbolTypes>> &start, QVector<QVector<SymbolTypes>> &stop)
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
            if(firstSymbolIsCorrect){
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
                if (start.at(j) != stop.at(j) &&            //Symbol is not finsihed
                    start.at(j).contains(targetSymbol) &&   //Theres at least 1 symbol that can be changed
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

/*
bool CalculateSteps::findAndSwap(QVector<QVector<SymbolTypes> > &start, QVector<QVector<SymbolTypes> > &ziel) {
    auto symbolToText = [](SymbolTypes symbol) ->QString{
        if(symbol == Kreis)
            return "Kreis";
        if(symbol == Viereck)
            return "Viereck";
        if(symbol == Dreieck)
            return "Dreieck";
        return "";
    };
    for (int i = 0; i < start.size(); ++i) {
        qDebug() << i << "of" << start.size();
        if (start.at(i) != ziel.at(i)) {
            //We have not match between start and stop, we have to switch
            qDebug() << "Something to do"<< start.at(i) <<ziel.at(i);
            auto &startPair = start[i];
            auto &zielPair = ziel[i];
            SymbolTypes wrongSymbol{Undefined};
            SymbolTypes targetSymbol{Undefined};

            // Check if the first symbol needs to switch
            if (zielPair.contains(startPair.at(0))) {
                // First symbol is part of the final configuration, so the second entry is wrong
                wrongSymbol = startPair.at(1);
                targetSymbol = zielPair.at(0) == startPair.at(0) ? zielPair.at(1) : zielPair.at(0);
            } else {
                // Second symbol is part of the final configuration, so the first entry is wrong
                wrongSymbol = startPair.at(0);
                targetSymbol = zielPair.at(1) == startPair.at(1) ? zielPair.at(0) : zielPair.at(1);
            }

            qDebug() << "Wrong symbol" <<wrongSymbol;
            qDebug() << "targetSymbol"<< targetSymbol;

            for (int j = 0; j < start.size(); ++j) {
                if(i == j){
                    continue;
                }
                qDebug() << "is not finished? " << (start.at(j) != ziel.at(j));
                qDebug() << "Statue has symbol we need?"<< start.at(j).contains(targetSymbol) << start.at(j) << targetSymbol;
                qDebug() << "Symbol is not part of the solution" << (!ziel.at(j).contains(targetSymbol)) <<ziel.at(j) << targetSymbol;
                qDebug() << "there are currently 2 symbols" << (start.at(j).count(targetSymbol) == 2) << start.at(j).count(targetSymbol) << Qt::endl;
                //if (start.at(j) != ziel.at(j) && start.at(j).contains(targetSymbol) && !ziel.at(j).contains(targetSymbol)) {
                if (start.at(j) != ziel.at(j) && // Symbol is not finsihed
                    start.at(j).contains(targetSymbol) &&//theres at least 1 symbol that can be changed
                    (!ziel.at(j).contains(targetSymbol) || //Symbol is not part of the solution
                    //if it is part of the solution, there are currently 2 of those symbols to take one from
                     start.at(j).count(targetSymbol) == 2))
                {
                // if (start.at(j) != ziel.at(j) && start.at(j).contains(targetSymbol)) {
                //     if(!ziel.at(j).contains(targetSymbol) || ziel.a) {
                    qDebug() << "Remove at Statue "<< i+1  << "Symbol" << symbolToText(wrongSymbol);
                    qDebug() << "Add at Statue "<< i+1  << "Symbol" << symbolToText(targetSymbol);
                    qDebug() << "Remove at Statue "<< j+1  << "Symbol" << symbolToText(targetSymbol);
                    qDebug() << "Add at Statue "<< j+1  << "Symbol" << symbolToText(wrongSymbol);

                    start[j].removeOne(targetSymbol);
                    start[j].append(wrongSymbol);
                    start[i].removeOne(wrongSymbol);
                    start[i].append(targetSymbol);
                    m_tauschOperationen.append(QPair<int,SymbolTypes>{i,wrongSymbol});
                    m_tauschOperationen.append(QPair<int,SymbolTypes>{j,targetSymbol});
                    qDebug() << "---";
                    return true;
                    }
                // }
            }
        }
    }
    return false;
}
*/

/* relict 1
    //Check if first Smybol needs to switch
    if(zielPair.contains(startPair.at(0))) {
        //Symbol part of finished configuration
        //We have something to change, so 2nd entry is wrong
        wrongSymbol = startPair.at(1);
        //Need to ID Missing symbol
        int indexCorrectPair = zielPair.indexOf(startPair.at(0));
        targetSymbol = indexCorrectPair == 0 ? zielPair.at(1) : zielPair.at(0);
    } else {
        //2nd Symbol needs is part of end configuration
        //so 1st entry is wrong
        wrongSymbol = startPair.at(0);
        //Need to ID Missing symbol
        int indexCorrectPair = zielPair.indexOf(startPair.at(1));
        targetSymbol = indexCorrectPair == 0 ? zielPair.at(0) : zielPair.at(1);
    }

*/
