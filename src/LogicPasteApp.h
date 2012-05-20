#ifndef LOGICPASTEAPP_H
#define LOGICPASTEAPP_H

#include <bb/cascades/Application>
#include <bb/cascades/QListDataModel>
#include <QtNetwork/QNetworkAccessManager>

#include "Pastebin.h"

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
    void onRefreshUserDetails();
    void onCreateAccount();

    void onProcessLogin(QString username, QString password);
    void onRefreshHistory();
    void onRefreshTrending();

private slots:
    void onLoginComplete();
    void onLoginFailed(QString message);
    void onUserDetailsAvailable(PasteUser pasteUser);
    void onHistoryAvailable(QList<PasteListing> *pasteList);
    void onTrendingAvailable(QList<PasteListing> *pasteList);

signals:
    void loginFailed(QString message);
    void settingsUpdated();

private:
    void refreshPasteListing(Page *page, QMapListDataModel *dataModel, QList<PasteListing> *pasteList);

    NavigationPane *navigationPane_;
    Page *pastePage_;
    Page *historyPage_;
    Page *trendingPage_;
    Page *settingsPage_;

    QMapListDataModel historyModel_;
    QMapListDataModel trendingModel_;

    Pastebin pastebin_;
};

#endif // LOGICPASTEAPP_H
