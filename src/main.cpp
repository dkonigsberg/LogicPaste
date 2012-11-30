#include <QtCore/QObject>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

#include <bb/cascades/Application>

#include "LogicPasteApp.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{    
    Application app(argc, argv);

    QTranslator translator;
    QString locale_string = QLocale().name(); 
    QString filename = QString( "LogicPaste_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    LogicPasteApp mainApp;
    
    return Application::exec();
}
