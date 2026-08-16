#ifndef GHOSTPLAYERSLOT_H
#define GHOSTPLAYERSLOT_H

#include <QObject>
#include <QString>

// One player slot in the Ghost Phase Helper's 6-slot roster: name, class,
// and a free-text note for the ghost/fashion callout. Purely manual entry -
// no Bungie API lookup (out of scope).
class GhostPlayerSlot : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString playerName READ playerName WRITE setPlayerName NOTIFY playerNameChanged FINAL)
    Q_PROPERTY(int classType READ classType WRITE setClassType NOTIFY classTypeChanged FINAL)
    Q_PROPERTY(QString manualNote READ manualNote WRITE setManualNote NOTIFY manualNoteChanged FINAL)
public:
    explicit GhostPlayerSlot(QObject *parent = nullptr);

    [[nodiscard]] QString playerName() const;
    void setPlayerName(const QString &name);

    [[nodiscard]] int classType() const;
    void setClassType(int type);

    [[nodiscard]] QString manualNote() const;
    void setManualNote(const QString &note);

public slots:
    void reset();

signals:
    void playerNameChanged();
    void classTypeChanged();
    void manualNoteChanged();

private:
    QString m_playerName;
    int m_classType = 0;
    QString m_manualNote;
};

#endif // GHOSTPLAYERSLOT_H
