#include "ghostphasehelper.h"

#include "ghostplayerslot.h"

GhostPhaseHelper::GhostPhaseHelper(QObject *parent)
    : QObject{parent}
{
    for (int i = 0; i < PlayerCount; ++i)
        m_players.append(new GhostPlayerSlot(this));
}

QList<QObject *> GhostPhaseHelper::players() const
{
    return m_players;
}

void GhostPhaseHelper::reset()
{
    for (auto *player : m_players)
        static_cast<GhostPlayerSlot *>(player)->reset();
}
