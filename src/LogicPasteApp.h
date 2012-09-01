#ifndef LOGICPASTEAPP_H
#define LOGICPASTEAPP_H

#include <bb/cascades/Application>
#include <QtNetwork/QNetworkAccessManager>

#include "Pastebin.h"
#include "PasteModel.h"

using namespace bb::cascades;

// Forward declarations
namespace bb { namespace cascades {
class NavigationPane;
class Page;
class Sheet;
} }

class FormatDropDown;

class LogicPasteApp : public QObject {
    Q_OBJECT

public:
    LogicPasteApp();

public slots:
    void onRequestLogin();
    void onRequestLogout();
    void onCreateAccount();

    void onProcessLogin(QString username, QString password);
    void onSubmitPaste();
    void onOpenHistoryPaste(QString pasteUrl, QString format);
    void onOpenTrendingPaste(QString pasteUrl, QString format);
    void onOpenPasteInBrowser(QString pasteUrl);
    void onCopyText(QString text);

private slots:
    void onLoginComplete(QString apiKey);
    void onLoginFailed(QString message);
    void onLoginCanceled();
    void onPasteComplete(QString pasteUrl);
    void onPasteFailed(QString message);
    void onUserDetailsUpdated();
    void onUserAvatarUpdated();
    void onActivePaneChanged(bb::cascades::AbstractPane *activePane);
    void onFormattedPasteAvailable(QString pasteUrl, QString html);

signals:
    void loginFailed(QString message);
    void settingsUpdated();

private:
    Page *pastePage_;
    NavigationPane *historyNav_;
    Page *historyPage_;
    NavigationPane *trendingNav_;
    Page *trendingPage_;
    Page *settingsPage_;
    Sheet *loginSheet_;

    PasteModel *pasteModel_;

    static FormatDropDown* replaceDropDown(Page *page, const QString& objectName);
    void openPaste(NavigationPane *nav, QString pasteUrl, QString format);
};

#endif // LOGICPASTEAPP_H
