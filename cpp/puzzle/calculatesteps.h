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

    // Challenge Mode: default distribution always targets fromBaseSymbol(inner)
    // - one of the 3 MIXED shapes (Kegel/Zylinder/Prisma), since that formula
    // never includes the statue's own inner symbol. "Varied Geometry" needs
    // all 6 shapes used across the encounter, so the only shapes default can
    // never produce are the 3 PURE ones (Pyramide/Wuerfel/Kugel) - Challenge
    // Mode exists purely to redirect a statue toward one of those instead.
    // Same reasoning as CalculateInsideSteps::calculateStepsChallenge().
    // No inner-symbol parameters here (unlike calculateSteps()) - the
    // target is given explicitly instead of derived from
    // fromBaseSymbol(inner), so inner never enters the actual swap math,
    // only checkIsValidChallenge()'s validation. Precondition: callers
    // must call checkIsValidChallenge() with the matching inner/target
    // arguments first.
    Q_INVOKABLE void calculateStepsChallenge(SymbolTypes outerStatue1,
                                              SymbolTypes outerStatue2,
                                              SymbolTypes outerStatue3,
                                              SymbolTypes challengeTarget1,
                                              SymbolTypes challengeTarget2,
                                              SymbolTypes challengeTarget3);
    // Inner symbols must be valid/pairwise-distinct, the 3 challenge targets
    // must be defined and balanced (each base symbol appears exactly twice
    // across them), AND no target may contain its own statue's inner symbol
    // - mechanically impossible, since a statue only ever holds/gives away
    // copies of its own inner symbol and could never receive one back (only
    // 2 copies of any base symbol exist in total, and the statue already
    // holds both after the swap sequence normalizes it).
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
