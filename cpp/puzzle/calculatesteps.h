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
    // Bumps on every calculateSteps()/reset() call - unlike numberOfSteps,
    // whose VALUE can legitimately repeat across calls, so QML can't rely
    // on it alone to know a recalculation happened.
    Q_PROPERTY(int calculationVersion READ calculationVersion NOTIFY calculationVersionChanged FINAL)
public:
    enum SymbolTypes {
        Undefined = 0,
        Dreieck = 1,
        Viereck = 2,
        Kreis = 3,
        Kegel = 4,
        Wuerfel = 5,
        Zylinder = 6,
        Pyramide = 7,
        Prisma = 8,
        Kugel = 9
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
    // The 3D shape `statue` actually holds right after step `step`
    // completes - not the overall final target (see targetShapeForStatue()).
    Q_INVOKABLE SymbolTypes shapeAfterStep(int step, int statue);
    // The 3D shape statue `statue` needs to end up as (its dissection
    // target), cached from the last calculateSteps() call. Undefined if
    // the index is out of range or nothing has been calculated yet.
    Q_INVOKABLE [[nodiscard]] SymbolTypes targetShapeForStatue(int statue) const;
    // False before any calculateSteps() call, and after one that got stuck
    // before reaching the target (no valid swap partner found - the
    // computed instruction list is then incomplete). True once a
    // calculateSteps() call actually reached the target.
    Q_INVOKABLE [[nodiscard]] bool isSolved() const;

    Q_INVOKABLE bool checkIsValid(SymbolTypes innerStatue1,
                                  SymbolTypes innerStatue2,
                                  SymbolTypes innerStatue3,
                                  SymbolTypes outerStatue1,
                                  SymbolTypes outerStatue2,
                                  SymbolTypes outerStatue3);

    // Challenge Mode: default target (fromBaseSymbol(inner)) is always one
    // of the 3 mixed shapes, so Challenge exists to redirect a statue
    // toward a pure one instead - same reasoning as
    // CalculateInsideSteps::calculateStepsChallenge(). No inner-symbol
    // params (unlike calculateSteps()) - target is given explicitly, inner
    // only matters for checkIsValidChallenge()'s validation. Precondition:
    // checkIsValidChallenge() with matching inner/target args first.
    Q_INVOKABLE void calculateStepsChallenge(SymbolTypes outerStatue1,
                                              SymbolTypes outerStatue2,
                                              SymbolTypes outerStatue3,
                                              SymbolTypes challengeTarget1,
                                              SymbolTypes challengeTarget2,
                                              SymbolTypes challengeTarget3);
    // Inner symbols valid/distinct, targets defined + balanced, and no
    // target contains its own statue's inner symbol - mechanically
    // impossible (a statue only ever holds/gives away its own symbol).
    Q_INVOKABLE bool checkIsValidChallenge(SymbolTypes innerStatue1,
                                            SymbolTypes innerStatue2,
                                            SymbolTypes innerStatue3,
                                            SymbolTypes challengeTarget1,
                                            SymbolTypes challengeTarget2,
                                            SymbolTypes challengeTarget3);

    [[nodiscard]] int calculationVersion() const { return m_calculationVersion; }

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
