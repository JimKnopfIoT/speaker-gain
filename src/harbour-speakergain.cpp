/*
  harbour-speakergain — main.cpp
  Copyright (C) 2026  harbour-speakergain contributors — GPLv3 or later.

  Sailfish OS binds the alarm's loudness to a profiled key of the general
  profile and ships no control for it, so the alarm ignores the ringtone
  slider, the volume keys and the profile switch alike. This app sets that
  key — and tells the user, per value, whether an update will keep it.
*/
#include <QCoreApplication>
#include <QGuiApplication>
#include <QLocale>
#include <QSettings>
#include <QTranslator>
#include <QQmlContext>
#include <QQuickView>
#include <QScopedPointer>

#include <cstdio>

#include <sailfishapp.h>

#include "speakergain.h"

int main(int argc, char *argv[])
{
    // The helper that applies a waiting volume once its output comes into use
    // (see SpeakerGain::setRouteVolume). No window, no QML.
    for (int i = 1; i < argc; ++i)
        if (qstrcmp(argv[i], "--apply-pending") == 0)
            return SpeakerGain::runApplyHelper(argc, argv);

    // Root side of the software volume for USB devices; run by
    // harbour-speakergain-softvol@.service, never by hand.
    for (int i = 1; i + 1 < argc; ++i)
        if (qstrcmp(argv[i], "--softvol") == 0) {
            QCoreApplication app(argc, argv);
            return SpeakerGain::runSoftVolHelper(QString::fromLatin1(argv[i + 1]));
        }

    // The report of About -> Diagnostics on the terminal, for whoever would
    // rather run one command than tap through the app.
    for (int i = 1; i < argc; ++i) {
        if (qstrcmp(argv[i], "--diagnostics") == 0) {
            QCoreApplication app(argc, argv);
            QCoreApplication::setOrganizationName(QStringLiteral("harbour-speakergain"));
            QCoreApplication::setApplicationName(QStringLiteral("harbour-speakergain"));
            SpeakerGain gain;
            fputs(gain.diagnosticReport().toUtf8().constData(), stdout);
            return 0;
        }
    }

    // A language for this window only.
    //
    // Screenshots for a listing have to be in English, and the way to get them
    // must not be to change the language of the phone: that closes every app,
    // pulls the ambience along and leaves someone's device in a state they did
    // not ask for. libsailfishapp picks the translation from the locale of the
    // process, so setting it here - before the application exists, and only in
    // this process - is enough. Nothing outside this app sees it.
    {
        QCoreApplication::setOrganizationName(QStringLiteral("harbour-speakergain"));
        QCoreApplication::setApplicationName(QStringLiteral("harbour-speakergain"));
        QSettings settings;
        if (settings.value(QStringLiteral("forceEnglish"), false).toBool()) {
            qputenv("LANGUAGE", "en");
            qputenv("LANG", "en_GB.utf8");
            qputenv("LC_MESSAGES", "en_GB.utf8");
            QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedKingdom));
        }
    }

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    // About shows Qt.application.version; nothing set it before.
    app->setApplicationVersion(QStringLiteral(APP_VERSION "-" APP_RELEASE));

    QScopedPointer<QQuickView> view(SailfishApp::createView());

    // libsailfishapp installs the device-locale catalogue when the view is
    // created, and the translator installed last is the one asked first. So an
    // English catalogue laid on top wins - no removing, no guessing at who owns
    // what. The same way harbour-xmatic switches between its languages.
    {
        QSettings settings;
        if (settings.value(QStringLiteral("forceEnglish"), false).toBool()) {
            QTranslator *english = new QTranslator(app.data());
            if (english->load(QStringLiteral("harbour-speakergain-en"),
                              QStringLiteral("/usr/share/harbour-speakergain/translations")))
                app->installTranslator(english);
            else {
                qWarning("speakergain: no English catalogue found; following the device");
                delete english;
            }
        }
    }

    SpeakerGain gain;
    view->rootContext()->setContextProperty(QStringLiteral("speakergain"), &gain);

    view->setSource(SailfishApp::pathToMainQml());
    view->show();
    return app->exec();
}
