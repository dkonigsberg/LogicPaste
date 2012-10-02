#ifndef PASTEFORMATTER_H
#define PASTEFORMATTER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QProcess>

class PasteFormatter : public QObject
{
    Q_OBJECT
public:
    PasteFormatter(QObject *parent=0);
    virtual ~PasteFormatter();

public slots:
    void formatPaste(const QString& pasteKey, const QString& format, const QByteArray& rawPaste);

signals:
    void pasteFormatted(const QString& pasteKey, const QString& html);
    void formatError();

private slots:
    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);
};

#endif // PASTEFORMATTER_H
