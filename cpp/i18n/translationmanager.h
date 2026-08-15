#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <memory>

#include <QObject>
#include <QString>

class QQmlEngine;
class QTranslator;

// Installs/swaps the app's runtime QTranslator and asks the QML engine to
// re-evaluate every qsTr() binding, so the flag-icon language toggle in
// main.qml's header takes effect immediately without a restart. Persists
// the chosen language via QSettings so it survives across app launches.
class TranslationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY currentLanguageChanged FINAL)
public:
    explicit TranslationManager(QObject *parent = nullptr);
    ~TranslationManager() override;

    // Must be called once, before QQmlApplicationEngine loads any QML, so
    // the initial UI already renders in the saved/system language without
    // needing a retranslate() pass. Loads the saved language from
    // QSettings, falling back to the system locale, falling back to "en".
    void initialize();

    // Must be called once, right after the QQmlApplicationEngine is
    // constructed - setLanguage() needs it to call retranslate().
    void setEngine(QQmlEngine *engine);

    QString currentLanguage() const { return m_currentLanguage; }

    // languageCode: "en" or "de". Unknown codes fall back to "en".
    Q_INVOKABLE void setLanguage(const QString &languageCode);

signals:
    void currentLanguageChanged();

private:
    void applyLanguage(const QString &languageCode);

    QQmlEngine *m_engine = nullptr;
    std::unique_ptr<QTranslator> m_translator;
    QString m_currentLanguage;
};

#endif // TRANSLATIONMANAGER_H
