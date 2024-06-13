#ifndef CALCULATESTEPS_H
#define CALCULATESTEPS_H

#include <QDebug>
#include <QObject>

class CalculateSteps : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int numberOfSteps READ numberOfSteps NOTIFY numberOfStepsChanged FINAL)
public:
    enum SymbolTypes {
        Undefined = 0,
        Dreieck = 1,
        Viereck,
        Kreis,
        Kegel,
        Wuerfel,
        Zylinder,
        Pyramide,
        Prisma,
        Kugel
    };
    Q_ENUM(SymbolTypes)
    explicit CalculateSteps(QObject *parent = nullptr);

    Q_INVOKABLE void calculateSteps(SymbolTypes innerStatue1,
                                    SymbolTypes innerStatue2,
                                    SymbolTypes innerStatue3,
                                    SymbolTypes outerStatue1,
                                    SymbolTypes outerStatue2,
                                    SymbolTypes outerStatue3);

    Q_INVOKABLE int numberOfSteps() { return m_swapOperations.size() / 2; }
    Q_INVOKABLE SymbolTypes getInstructionForStep(int step, int statue);

    Q_INVOKABLE bool checkIsValid(SymbolTypes innerStatue1,
                                  SymbolTypes innerStatue2,
                                  SymbolTypes innerStatue3,
                                  SymbolTypes outerStatue1,
                                  SymbolTypes outerStatue2,
                                  SymbolTypes outerStatue3);

public slots:
    void reset();

signals:
    void numberOfStepsChanged();

private:
    QVector<SymbolTypes> toBaseSymbols(SymbolTypes type);
    QVector<SymbolTypes> fromBaseSymbol(SymbolTypes type);
    void orderSymbolsInPairs(QVector<QVector<SymbolTypes>> &toOrder);

    bool isFinished(const QVector<QVector<SymbolTypes>> &start,
                    const QVector<QVector<SymbolTypes>> &ziel);

    bool findAndSwap(QVector<QVector<SymbolTypes>> &start, QVector<QVector<SymbolTypes>> &stop);

private:
    QVector<QPair<int, SymbolTypes>> m_swapOperations;
};

#endif // CALCULATESTEPS_H
