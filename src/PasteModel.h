#ifndef PASTEMODEL_H
#define PASTEMODEL_H

#include <QtCore/QObject>
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

    PasteUser pasteUserDetails() const;
    DataModel* historyModel() const;
    DataModel* trendingModel() const;

public slots:
    void refreshUserDetails();
    void refreshHistory();
    void refreshTrending();

signals:
    void userDetailsUpdated(PasteUser pasteUser);
    void historyUpdating();
    void historyUpdated();
    void trendingUpdating();
    void trendingUpdated();

private slots:
    void onLoginComplete(QString apiKey);
    void onUserDetailsAvailable(PasteUser pasteUser);
    void onHistoryAvailable(QList<PasteListing> *pasteList);
    void onTrendingAvailable(QList<PasteListing> *pasteList);

private:
    void saveUserDetails();
    void loadUserDetails();
    void loadPasteDatabase();
    void refreshPasteListing(QMapListDataModel *dataModel, QList<PasteListing> *pasteList);
    void updatePasteTable(QString tableName, QList<PasteListing> *pasteList);
    void loadPasteTable(QString tableName, QMapListDataModel *dataModel);

    Pastebin pastebin_;
    QSqlDatabase pasteDb_;
    PasteUser pasteUser_;
    QMapListDataModel *historyModel_;
    QMapListDataModel *trendingModel_;
};

#endif // PASTEMODEL_H
