#ifndef PASTEMODEL_H
#define PASTEMODEL_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtSql/QtSql>
#include <bb/cascades/QListDataModel>
#include "Pastebin.h"

using namespace bb::cascades;

class PasteModel : public QObject {
    Q_OBJECT
public:
    explicit PasteModel(QObject *parent=0);
    virtual ~PasteModel();

    Q_INVOKABLE bool isAuthenticated();
    void logout();

    Pastebin* pastebin();

    DataModel* historyModel() const;
    DataModel* trendingModel() const;

    PasteListing pasteListing(const QString& pasteKey) const;

    void requestPaste(const QString& pasteKey);

public slots:
    void refreshUserDetails();
    void refreshHistory();
    void refreshTrending();

signals:
    void userDetailsUpdated();
    void userAvatarUpdated();
    void historyUpdating();
    void historyUpdated();
    void trendingUpdating();
    void trendingUpdated();
    void pasteAvailable(PasteListing pasteListing, QByteArray rawPaste);
    void pasteError(PasteListing pasteListing);

private slots:
    void onLoginComplete(QString apiKey);
    void onUserDetailsUpdated();
    void onHistoryAvailable(QList<PasteListing> *pasteList);
    void onTrendingAvailable(QList<PasteListing> *pasteList);
    void onRawPasteAvailable(QString pasteKey, QByteArray rawPaste);
    void onRawPasteError(QString pasteKey);

private:
    void loadPasteDatabase();
    void refreshPasteListing(QMapListDataModel *dataModel, QList<PasteListing> *pasteList);
    void updatePasteTable(QString tableName, QList<PasteListing> *pasteList);
    void loadPasteTable(QString tableName, QMapListDataModel *dataModel);
    static PasteListing createFakePasteListing(const QString& pasteKey);

    Pastebin pastebin_;
    QSqlDatabase pasteDb_;
    QMapListDataModel *historyModel_;
    QMapListDataModel *trendingModel_;
    QHash<QString, PasteListing> pasteListingMap_;
};

#endif // PASTEMODEL_H
