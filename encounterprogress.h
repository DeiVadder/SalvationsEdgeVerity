#ifndef ENCOUNTERPROGRESS_H
#define ENCOUNTERPROGRESS_H

#include <QObject>
#include <QSet>
#include <QVector>

#include "calculatesteps.h"

// Tracks which of the 6 outside 3D shapes have been used to escape the solo
// rooms across the whole encounter (all loops), for the "Varied Geometry"
// challenge rule: no shape may repeat until all 6 have been used at least
// once. Deliberately a separate lifetime from CalculateSteps/
// CalculateInsideSteps's per-loop reset() - must survive those.
class EncounterProgress : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool challengeModeEnabled READ challengeModeEnabled WRITE setChallengeModeEnabled
                   NOTIFY challengeModeEnabledChanged FINAL)
    Q_PROPERTY(int usedShapeCount READ usedShapeCount NOTIFY usedShapesChanged FINAL)
public:
    explicit EncounterProgress(QObject *parent = nullptr);

    Q_INVOKABLE void markShapeUsed(CalculateSteps::SymbolTypes shape);
    Q_INVOKABLE bool isShapeAvailable(CalculateSteps::SymbolTypes shape) const;
    Q_INVOKABLE QVector<CalculateSteps::SymbolTypes> availableShapes() const;
    Q_INVOKABLE void resetEncounter();

    bool challengeModeEnabled() const;
    void setChallengeModeEnabled(bool enabled);
    int usedShapeCount() const;

signals:
    void challengeModeEnabledChanged();
    void usedShapesChanged();

private:
    static bool isTrackedShape(CalculateSteps::SymbolTypes shape);

    bool m_challengeModeEnabled = false;
    QSet<CalculateSteps::SymbolTypes> m_usedShapes;
};

#endif // ENCOUNTERPROGRESS_H
