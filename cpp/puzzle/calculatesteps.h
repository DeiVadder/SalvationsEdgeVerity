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
    // Bumped on every calculateSteps()/reset() call. numberOfSteps alone
    // isn't a reliable QML re-evaluation trigger for bindings that only
    // call plain Q_INVOKABLE methods (e.g. targetShapeForStatue(),
    // getInstructionForStep()) - two different valid (inner,outer)
    // combinations can produce the same step count with different actual
    // instructions, and QML only re-fires a dependent binding when a
    // watched property's VALUE changes, not merely when its NOTIFY signal
    // fires with an unchanged value. This counter always changes, so it's
    // always safe to depend on for that trick (see SolutionPanel.qml).
    Q_PROPERTY(int calculationVersion READ calculationVersion NOTIFY calculationVersionChanged FINAL)
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
    // The 3D shape statue `statue` needs to end up as (its dissection
    // target), cached from the last calculateSteps() call. Undefined if
    // the index is out of range or nothing has been calculated yet.
    Q_INVOKABLE SymbolTypes targetShapeForStatue(int statue) const;
    // False before any calculateSteps() call, and after one that got stuck
    // before reaching the target (no valid swap partner found - the
    // computed instruction list is then incomplete). True once a
    // calculateSteps() call actually reached the target.
    Q_INVOKABLE bool isSolved() const;

    Q_INVOKABLE bool checkIsValid(SymbolTypes innerStatue1,
                                  SymbolTypes innerStatue2,
                                  SymbolTypes innerStatue3,
                                  SymbolTypes outerStatue1,
                                  SymbolTypes outerStatue2,
                                  SymbolTypes outerStatue3);

    int calculationVersion() const { return m_calculationVersion; }

public slots:
    void reset();

signals:
    void numberOfStepsChanged();
    void calculationVersionChanged();

private:
    void bumpCalculationVersion();

    std::unique_ptr<SymbolSwapEngine> m_engine;
    QVector<SymbolTypes> m_targetShapePerStatue;
    int m_calculationVersion = 0;
};

#endif // CALCULATESTEPS_H
