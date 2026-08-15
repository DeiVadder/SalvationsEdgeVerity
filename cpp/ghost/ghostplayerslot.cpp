#include "ghostplayerslot.h"

GhostPlayerSlot::GhostPlayerSlot(QObject *parent)
    : QObject{parent}
{}

QString GhostPlayerSlot::playerName() const
{
    return m_playerName;
}

void GhostPlayerSlot::setPlayerName(const QString &name)
{
    if (m_playerName == name) {
        return;
    }
    m_playerName = name;
    playerNameChanged();
}

int GhostPlayerSlot::classType() const
{
    return m_classType;
}

void GhostPlayerSlot::setClassType(int type)
{
    if (m_classType == type) {
        return;
    }
    m_classType = type;
    classTypeChanged();
}

QString GhostPlayerSlot::manualNote() const
{
    return m_manualNote;
}

void GhostPlayerSlot::setManualNote(const QString &note)
{
    if (m_manualNote == note) {
        return;
    }
    m_manualNote = note;
    manualNoteChanged();
}

void GhostPlayerSlot::reset()
{
    setPlayerName(QString());
    setClassType(0);
    setManualNote(QString());
}
