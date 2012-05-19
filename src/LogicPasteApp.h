#ifndef LOGICPASTEAPP_H
#define LOGICPASTEAPP_H

#include <bb/cascades/Application>
#include <QtNetwork/QNetworkAccessManager>

using namespace bb::cascades;

// Forward declarations
namespace bb { namespace cascades {
class NavigationPane;
class Page;
} }


class LogicPasteApp : public QObject {
    Q_OBJECT

public:
    LogicPasteApp();

    Q_INVOKABLE QString getSettingValue(const QString &keyName);

public slots:
    void onRequestLogin();
    void onRequestLogout();
    void onProcessLogin(QString username, QString password);
    void onCreateAccount();

    void onPasteBinApiLogin();

signals:
    void loginFailed(QString message);
    void settingsUpdated();

private:
    NavigationPane *navigationPane_;
    Page *pastePage_;
    Page *historyPage_;
    Page *settingsPage_;

    QNetworkAccessManager accessManager_;

    static const char* const pasteBinLoginUrl_;
    static const char* const pasteBinDevKey_;
};

#endif // LOGICPASTEAPP_H
