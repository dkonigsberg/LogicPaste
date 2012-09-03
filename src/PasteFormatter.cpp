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
    int fd_in[2];
    int ret = ::pipe(fd_in);
    if(ret == -1) {
        qWarning("Input pipe error (%s)", qPrintable(qt_error_string(errno)));
        emit formatError();
        return;
    }
    ::fcntl(fd_in[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(fd_in[1], F_SETFD, FD_CLOEXEC);

    int fd_out[2];
    ret = ::pipe(fd_out);
    if(ret == -1) {
        qWarning("Output pipe error (%s)", qPrintable(qt_error_string(errno)));
        emit formatError();
        return;
    }
    ::fcntl(fd_out[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(fd_out[1], F_SETFD, FD_CLOEXEC);

    QSocketNotifier *outNotifier = new QSocketNotifier(fd_out[0], QSocketNotifier::Read, this);
    outNotifier->setProperty("pasteKey", pasteKey);
    connect(outNotifier, SIGNAL(activated(int)), this, SLOT(onOutActivated(int)));

    readBufferMap_[fd_out[0]] = new QByteArray();

    const int fd_count = 3;
    int fd_map[fd_count];
    fd_map[0] = fd_in[0];
    fd_map[1] = fd_out[1];
    fd_map[2] = STDERR_FILENO;

    struct inheritance inherit;
    memset(&inherit, 0, sizeof(inherit));
    inherit.flags |= SPAWN_CHECK_SCRIPT;
    inherit.flags |= SPAWN_SETSIGDEF;
    sigaddset(&inherit.sigdefault, SIGPIPE);

    AppSettings *appSettings = AppSettings::instance();
    QStringList options;
    options << "encoding=utf-8";
    options << "full";
    if(appSettings->formatterLineNumbering()) {
        options << "linenos";
    }
    options << QString("style=%1").arg(appSettings->formatterStyle());
    qDebug() << options;

    QStringList arguments;
    arguments << "/usr/bin/python3.2" << "app/native/lib/pygmentize";
    arguments << "-l" << format;
    arguments << "-f" << "html";
    arguments << "-O" << options.join(",");

    char **argv = new char *[arguments.count() + 1];
    argv[arguments.count()] = 0;

    for (int i = 0; i < arguments.count(); ++i) {
        QString arg = arguments.at(i);
        argv[i] = ::strdup(arg.toLocal8Bit().constData());
    }

    qDebug() << "Running formatter:" << arguments;

    pid_t pid = spawn(argv[0], fd_count, fd_map, &inherit, argv, NULL);
    int lastErrno = errno;

    for (int i = 0; i < arguments.count(); ++i) {
        free(argv[i]);
    }
    delete [] argv;

    if(pid == -1) {
        qWarning("Spawn error (%s)", qPrintable(qt_error_string(lastErrno)));
        outNotifier->setEnabled(false);
        ::close(fd_in[0]); ::close(fd_in[1]);
        ::close(fd_out[0]); ::close(fd_out[1]);
        delete outNotifier;
        emit formatError();
        return;
    }
    else {
        qDebug() << "Spawned:" << pid;
    }

    // Close the other ends of the pipes
    ::close(fd_out[1]);
    ::close(fd_in[0]);

    int n = ::write(fd_in[1], rawPaste.data(), rawPaste.size());
    if(n < rawPaste.size()) {
        if(n == -1) {
            qWarning("Write error (%s)", qPrintable(qt_error_string(errno)));
        }
        else {
            qWarning() << "Write error:" << n << "<" << rawPaste.size();
        }
        outNotifier->setEnabled(false);
        delete outNotifier;
        emit formatError();
    }
    qDebug() << "Wrote" << n << "bytes";

    ::close(fd_in[1]);
}

void PasteFormatter::onOutActivated(int socket)
{
    qDebug().nospace() << "PasteFormatter::onOutActivated(" << socket << ")";
    QSocketNotifier *notifier = qobject_cast<QSocketNotifier*>(sender());
    notifier->setEnabled(false);

    int nbytes = 0;
    qint64 available = 0;
    if (::ioctl(socket, FIONREAD, (char *) &nbytes) >= 0) {
        available = (qint64) nbytes;
    }

    if(available > 0) {
        QByteArray data;
        data.reserve(available);
        char *raw = data.data();
        int n = ::read(socket, raw, available);
        if(n > 0) {
            QString pasteKey = notifier->property("pasteKey").toString();

            QByteArray *readBuffer = readBufferMap_[socket];
            if(readBuffer) {
                readBuffer->append(raw, n);
            }
        }
        notifier->setEnabled(true);
    }
    else {
        ::close(socket);
        QByteArray *readBuffer = readBufferMap_.take(socket);
        if(readBuffer) {
            const QString pasteKey = notifier->property("pasteKey").toString();
            const QString html = QString::fromUtf8(readBuffer->constData(), readBuffer->size());
            delete readBuffer;

            if(html.length() > 0) {
                qDebug() << "Processed HTML:" << html.length();
                emit pasteFormatted(pasteKey, html);
            }
            else {
                emit formatError();
            }
        } else {
            emit formatError();
        }
        notifier->deleteLater();
    }
}
