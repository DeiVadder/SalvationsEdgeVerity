#ifndef CALCULATESTEPS_H
#define CALCULATESTEPS_H

#include <memory>

#include <QDebug>
#include <QObject>
#include <QVector>

class SymbolSwapEngine;

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
    ~CalculateSteps() override;

    static QVector<SymbolTypes> toBaseSymbols(SymbolTypes type);
    static QVector<SymbolTypes> fromBaseSymbol(SymbolTypes type);
    static SymbolTypes pairToShape(SymbolTypes a, SymbolTypes b);

    // Precondition: callers must call checkIsValid() with the same arguments first.
    // calculateSteps() does not validate internally; invalid input will not hang
    // (findAndSwap's stuck-case break prevents that) but produces an incomplete
    // instruction set.
    Q_INVOKABLE void calculateSteps(SymbolTypes innerStatue1,
                                    SymbolTypes innerStatue2,
                                    SymbolTypes innerStatue3,
                                    SymbolTypes outerStatue1,
                                    SymbolTypes outerStatue2,
                                    SymbolTypes outerStatue3);

    Q_INVOKABLE int numberOfSteps();
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
    std::unique_ptr<SymbolSwapEngine> m_engine;
};

#endif // CALCULATESTEPS_H
