#include "translationmanager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QLocale>
#include <QQmlEngine>
#include <QSettings>
#include <QTranslator>

namespace {

QString detectDefaultLanguage()
{
    QSettings settings;
    QString saved = settings.value(QStringLiteral("Language/code")).toString();
    if (saved == QStringLiteral("en") || saved == QStringLiteral("de")) {
        return saved;
    }
    return QLocale::system().name().startsWith(QStringLiteral("de")) ? QStringLiteral("de")
                                                                      : QStringLiteral("en");
}

} // namespace

TranslationManager::TranslationManager(QObject *parent)
    : QObject(parent)
{
}

TranslationManager::~TranslationManager()
{
    if (m_translator) {
        QCoreApplication::removeTranslator(m_translator.get());
    }
}

void TranslationManager::initialize()
{
    applyLanguage(detectDefaultLanguage());
}

void TranslationManager::setEngine(QQmlEngine *engine)
{
    m_engine = engine;
}

void TranslationManager::applyLanguage(const QString &languageCode)
{
    if (m_translator) {
        QCoreApplication::removeTranslator(m_translator.get());
        m_translator.reset();
    }

    QString resolvedCode = languageCode;
    if (resolvedCode != QStringLiteral("en")) {
        auto translator = std::make_unique<QTranslator>();
        if (translator->load(QStringLiteral(":/i18n/salvationsedgeverity_%1.qm").arg(resolvedCode))) {
            QCoreApplication::installTranslator(translator.get());
            m_translator = std::move(translator);
        } else {
            qWarning() << "TranslationManager: no .qm found for" << resolvedCode << "- falling back to en";
            resolvedCode = QStringLiteral("en");
        }
    }

    m_currentLanguage = resolvedCode;
}

void TranslationManager::setLanguage(const QString &languageCode)
{
    if (languageCode == m_currentLanguage) {
        return;
    }

    applyLanguage(languageCode);

    QSettings settings;
    settings.setValue(QStringLiteral("Language/code"), m_currentLanguage);

    if (m_engine != nullptr) {
        m_engine->retranslate();
    }

    emit currentLanguageChanged();
}
