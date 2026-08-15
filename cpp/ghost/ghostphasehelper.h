#ifndef GHOSTPHASEHELPER_H
#define GHOSTPHASEHELPER_H

#include <QList>
#include <QObject>

// Owns the 6-player roster for the Ghost Phase Helper. Phase a.1: roster +
// manual entry only. Bungie-API lookup (lookupGhostForSlot, m_apiClient)
// is added in phase a.3 once BungieApiClient exists.
class GhostPhaseHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject *> players READ players CONSTANT FINAL)
public:
    static constexpr int PlayerCount = 6;

    explicit GhostPhaseHelper(QObject *parent = nullptr);

    [[nodiscard]] QList<QObject *> players() const;

public slots:
    void reset();

private:
    QList<QObject *> m_players;
};

#endif // GHOSTPHASEHELPER_H
