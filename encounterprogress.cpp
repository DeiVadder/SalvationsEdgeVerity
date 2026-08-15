#include "encounterprogress.h"

EncounterProgress::EncounterProgress(QObject *parent)
    : QObject{parent}
{}

bool EncounterProgress::isTrackedShape(CalculateSteps::SymbolTypes shape)
{
    switch (shape) {
    case CalculateSteps::Kegel:
    case CalculateSteps::Wuerfel:
    case CalculateSteps::Zylinder:
    case CalculateSteps::Pyramide:
    case CalculateSteps::Prisma:
    case CalculateSteps::Kugel:
        return true;
    default:
        return false;
    }
}

void EncounterProgress::markShapeUsed(CalculateSteps::SymbolTypes shape)
{
    if (!isTrackedShape(shape))
        return;

    // "May not repeat until all 6 have been used" - once every tracked shape
    // has appeared, the next mark starts a fresh cycle.
    if (m_usedShapes.size() >= 6)
        m_usedShapes.clear();

    m_usedShapes.insert(shape);
    usedShapesChanged();
}

bool EncounterProgress::isShapeAvailable(CalculateSteps::SymbolTypes shape) const
{
    return !m_usedShapes.contains(shape);
}

QVector<CalculateSteps::SymbolTypes> EncounterProgress::availableShapes() const
{
    static const QVector<CalculateSteps::SymbolTypes> allShapes = {CalculateSteps::Kegel,
                                                                    CalculateSteps::Wuerfel,
                                                                    CalculateSteps::Zylinder,
                                                                    CalculateSteps::Pyramide,
                                                                    CalculateSteps::Prisma,
                                                                    CalculateSteps::Kugel};
    QVector<CalculateSteps::SymbolTypes> result;
    for (auto shape : allShapes) {
        if (!m_usedShapes.contains(shape))
            result.append(shape);
    }
    return result;
}

void EncounterProgress::resetEncounter()
{
    m_usedShapes.clear();
    usedShapesChanged();
}

bool EncounterProgress::challengeModeEnabled() const
{
    return m_challengeModeEnabled;
}

void EncounterProgress::setChallengeModeEnabled(bool enabled)
{
    if (m_challengeModeEnabled == enabled)
        return;
    m_challengeModeEnabled = enabled;
    challengeModeEnabledChanged();
}

int EncounterProgress::usedShapeCount() const
{
    return m_usedShapes.size();
}
