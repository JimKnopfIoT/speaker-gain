/*
  harbour-speakergain — main.cpp
  Copyright (C) 2026  harbour-speakergain contributors — GPLv3 or later.

  Sailfish OS binds the alarm's loudness to a profiled key of the general
  profile and ships no control for it, so the alarm ignores the ringtone
  slider, the volume keys and the profile switch alike. This app sets that
  key — and tells the user, per value, whether an update will keep it.
*/
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QScopedPointer>

#include <sailfishapp.h>

#include "speakergain.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationVersion(QStringLiteral(APP_VERSION));

    QScopedPointer<QQuickView> view(SailfishApp::createView());

    SpeakerGain gain;
    view->rootContext()->setContextProperty(QStringLiteral("speakergain"), &gain);

    view->setSource(SailfishApp::pathToMainQml());
    view->show();
    return app->exec();
}
