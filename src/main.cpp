#include <QtCore/QObject>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

#include <bb/cascades/Application>

#include <Qt/qdeclarativedebug.h>

#include "LogicPasteApp.h"

using namespace bb::cascades;

#ifndef QT_NO_DEBUG_OUTPUT
void logMessageHandler(QtMsgType type, const char *msg)
{
    QString debugdate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    switch (type)
    {
    case QtDebugMsg:
        debugdate += "[D]";
        break;
    case QtWarningMsg:
        debugdate += "[W]";
        break;
    case QtCriticalMsg:
        debugdate += "[C]";
        break;
    case QtFatalMsg:
        debugdate += "[F]";
        break;
    }

    fprintf(stderr, "%s %s\n", debugdate.toLatin1().data(), msg);

    if (QtFatalMsg == type) {
        abort();
    }
}
#endif

Q_DECL_EXPORT int main(int argc, char **argv)
{
#ifndef QT_NO_DEBUG_OUTPUT
    qInstallMsgHandler(logMessageHandler);
#endif

    Application app(argc, argv);

    QTranslator translator;
    QString locale_string = QLocale().name(); 
    QString filename = QString("LogicPaste_%1").arg(locale_string);
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator(&translator);
    }

    new LogicPasteApp(&app);
    
    return Application::exec();
}
