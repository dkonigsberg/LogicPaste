#include "PasteFormatter.h"

#include <QtCore/QDebug>
#include <QtCore/QSocketNotifier>
#include <QtCore/QStringList>

#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>
#include <errno.h>
#include <ioctl.h>

#include "AppSettings.h"

PasteFormatter::PasteFormatter(QObject *parent) : QObject(parent)
{
}

PasteFormatter::~PasteFormatter()
{
}

void PasteFormatter::formatPaste(const QString& pasteKey, const QString& format, const QByteArray& rawPaste)
{
    qDebug().nospace() << "PasteFormatter::formatPaste(" << pasteKey << ", " << format << ", [" << rawPaste.length() << "])";

    QProcess *process = new QProcess(this);
    connect(process, SIGNAL(started()), this, SLOT(onStarted()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
    process->setProperty("pasteKey", pasteKey);
    process->setProperty("rawPaste", rawPaste);


    QString program = "/usr/bin/python3.2";

    AppSettings *appSettings = AppSettings::instance();
    QStringList options;
    options << "encoding=utf-8";
    options << "full";
    if(appSettings->formatterLineNumbering()) {
        options << "linenos";
    }
    options << QString("style=%1").arg(appSettings->formatterStyle());

    QStringList arguments;
    arguments << "app/native/lib/pygmentize";
    arguments << "-l" << format;
    arguments << "-f" << "html";
    arguments << "-O" << options.join(",");

    process->start(program, arguments);
}

void PasteFormatter::onStarted()
{
    qDebug() << "PasteFormatter::onStarted()";
    QProcess *process = qobject_cast<QProcess*>(sender());
    const QByteArray rawPaste = process->property("rawPaste").toByteArray();
    process->write(rawPaste);
    process->closeWriteChannel();
}

void PasteFormatter::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug().nospace() << "PasteFormatter::onFinished(" << exitCode << ", " << exitStatus << ")";
    QProcess *process = qobject_cast<QProcess*>(sender());

    const QString pasteKey = process->property("pasteKey").toString();
    const QByteArray output = process->readAllStandardOutput();
    const QString html = QString::fromUtf8(output.constData(), output.size());

    if(html.length() > 0) {
        qDebug() << "Processed HTML:" << html.length();
        emit pasteFormatted(pasteKey, html);
    }
    else {
        qDebug() << QString::fromUtf8(process->readAllStandardError());
        emit formatError();
    }

    process->deleteLater();
}

void PasteFormatter::onError(QProcess::ProcessError error)
{
    qDebug().nospace() << "PasteFormatter::onError(" << error << ")";
    QProcess *process = qobject_cast<QProcess*>(sender());
    process->deleteLater();
    emit formatError();
}
