#include <bb/cascades/QListDataModel>
#include "PasteModel.h"

PasteModel::PasteModel(QObject *parent)
    : QObject(parent) {
    historyModel_ = new QMapListDataModel();
    trendingModel_ = new QMapListDataModel();

    loadUserDetails();
    loadPasteDatabase();

    loadPasteTable("user_pastes", historyModel_);
    loadPasteTable("trending_pastes", trendingModel_);

    connect(&pastebin_, SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    connect(&pastebin_, SIGNAL(userDetailsAvailable(PasteUser)), this, SLOT(onUserDetailsAvailable(PasteUser)));
    connect(&pastebin_, SIGNAL(historyAvailable(QList<PasteListing> *)), this, SLOT(onHistoryAvailable(QList<PasteListing> *)));
    connect(&pastebin_, SIGNAL(trendingAvailable(QList<PasteListing> *)), this, SLOT(onTrendingAvailable(QList<PasteListing> *)));
}

PasteModel::~PasteModel() {
    if(pasteDb_.isOpen()) {
        QSqlDatabase::removeDatabase("data/pasteData.db");
        pasteDb_.removeDatabase("QSQLITE");
        qDebug() << "Database closed";
    }

    disconnect(&pastebin_, SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    disconnect(&pastebin_, SIGNAL(userDetailsAvailable(PasteUser)), this, SLOT(onUserDetailsAvailable(PasteUser)));
    disconnect(&pastebin_, SIGNAL(historyAvailable(QList<PasteListing> *)), this, SLOT(onHistoryAvailable(QList<PasteListing> *)));
    disconnect(&pastebin_, SIGNAL(trendingAvailable(QList<PasteListing> *)), this, SLOT(onTrendingAvailable(QList<PasteListing> *)));

    delete historyModel_;
    delete trendingModel_;
}

void PasteModel::refreshUserDetails() {
    pastebin_.requestUserDetails();
}

void PasteModel::refreshHistory() {
    emit historyUpdating();
    pastebin_.requestHistory();
}

void PasteModel::refreshTrending() {
    emit trendingUpdating();
    pastebin_.requestTrending();
}

bool PasteModel::isAuthenticated() {
    return !pastebin_.apiKey().isEmpty();
}

Pastebin* PasteModel::pastebin() {
    return &pastebin_;
}

PasteUser PasteModel::pasteUserDetails() const {
    return pasteUser_;
}

DataModel* PasteModel::historyModel() const {
    return historyModel_;
}

DataModel* PasteModel::trendingModel() const {
    return trendingModel_;
}

void PasteModel::onLoginComplete(QString apiKey) {
    qDebug() << "PasteModel::onLoginComplete()";
    pasteUser_.setApiKey(apiKey);
    saveUserDetails();

    pastebin_.setApiKey(apiKey);
    pastebin_.requestUserDetails();
}

void PasteModel::onUserDetailsAvailable(PasteUser pasteUser) {
    pasteUser_ = pasteUser;
    saveUserDetails();
    emit userDetailsUpdated(pasteUser_);
}

void PasteModel::onHistoryAvailable(QList<PasteListing> *pasteList) {
    qDebug() << "PasteModel::onHistoryAvailable()";
    refreshPasteListing(historyModel_, pasteList);
    updatePasteTable("user_pastes", pasteList);
    delete pasteList;
    emit historyUpdated();
}

void PasteModel::onTrendingAvailable(QList<PasteListing> *pasteList) {
    qDebug() << "PasteModel::onTrendingAvailable()";
    refreshPasteListing(trendingModel_, pasteList);
    updatePasteTable("trending_pastes", pasteList);
    delete pasteList;
    emit trendingUpdated();
}

void PasteModel::saveUserDetails() {
    QSettings settings;
    settings.setValue("api_user_name", pasteUser_.username());
    settings.setValue("api_user_key", pasteUser_.apiKey());
    settings.setValue("user_avatar_url", pasteUser_.avatarUrl());
    settings.setValue("user_website", pasteUser_.website());
    settings.setValue("user_email", pasteUser_.email());
    settings.setValue("user_location", pasteUser_.location());
    settings.setValue("user_account_type", static_cast<int>(pasteUser_.accountType()));
    settings.setValue("user_format_short", pasteUser_.pasteFormatShort());
    settings.setValue("user_expiration", pasteUser_.pasteExpiration());
    settings.setValue("user_private", static_cast<int>(pasteUser_.pasteVisibility()));
}

void PasteModel::loadUserDetails() {
    QSettings settings;
    pasteUser_.setUsername(settings.value("api_user_name", "").toString());
    pasteUser_.setApiKey(settings.value("api_user_key", "").toString());
    pasteUser_.setAvatarUrl(settings.value("user_avatar_url", "").toString());
    pasteUser_.setWebsite(settings.value("user_website", "").toString());
    pasteUser_.setEmail(settings.value("user_email", "").toString());
    pasteUser_.setLocation(settings.value("user_location", "").toString());
    pasteUser_.setAccountType(static_cast<PasteUser::AccountType>(settings.value("user_account_type", 0).toInt()));
    pasteUser_.setPasteFormatShort(settings.value("user_format_short", "").toString());
    pasteUser_.setPasteExpiration(settings.value("user_expiration", "").toString());
    pasteUser_.setPasteVisibility(static_cast<PasteListing::Visibility>(settings.value("user_private", 0).toInt()));

    pastebin_.setApiKey(pasteUser_.apiKey());
}

void PasteModel::logout() {
    QSettings settings;
    settings.remove("api_user_name");
    settings.remove("api_user_key");
    settings.remove("user_avatar_url");
    settings.remove("user_website");
    settings.remove("user_email");
    settings.remove("user_location");
    settings.remove("user_account_type");
    settings.remove("user_format_short");
    settings.remove("user_expiration");
    settings.remove("user_private");

    pasteUser_ = PasteUser();

    pastebin_.setApiKey(NULL);
}

void PasteModel::loadPasteDatabase() {
    pasteDb_ = QSqlDatabase::addDatabase("QSQLITE");
    pasteDb_.setDatabaseName("data/pasteData.db");
    if(pasteDb_.open()) {
        qDebug() << "Database opened";

        QSqlQuery query(pasteDb_);

        QString queryString(
                "CREATE TABLE IF NOT EXISTS %1 ("
                "id INTEGER PRIMARY KEY, "
                "key TEXT, paste_date INTEGER, title TEXT, size INTEGER, "
                "expire_date INTEGER, visibility INTEGER, "
                "format_long TEXT, format_short TEXT, "
                "url TEXT, hits INTEGER)");

        query.prepare(queryString.arg("user_pastes"));
        if(!query.exec()) {
            qDebug() << "Unable to create user_pastes table:" << query.lastError();
            return;
        }

        query.prepare(queryString.arg("trending_pastes"));
        if(!query.exec()) {
            qDebug() << "Unable to create trending_pastes table:" << query.lastError();
            return;
        }

        qDebug() << "Database ready for use";
    }
    else {
        qDebug() << "Database error:" << pasteDb_.lastError();
    }
}

void PasteModel::refreshPasteListing(QMapListDataModel *dataModel, QList<PasteListing> *pasteList) {
    dataModel->clear();
    QVariantMap map;
    foreach(PasteListing paste, *pasteList) {
        map["title"] = paste.title();
        map["pasteDate"] = paste.pasteDate().toString(Qt::LocaleDate);
        map["format"] = paste.formatLong();

        if(paste.visibility() == PasteListing::Public) {
            map["imageSource"] = "asset:///images/item-paste-public.png";
        } else if(paste.visibility() == PasteListing::Unlisted) {
            map["imageSource"] = "asset:///images/item-paste-unlisted.png";
        } else if(paste.visibility() == PasteListing::Private) {
            map["imageSource"] = "asset:///images/item-paste-private.png";
        }

        map["pasteUrl"] = paste.url();
        map["pasteKey"] = paste.key();

        dataModel->append(map);
    }
}

void PasteModel::updatePasteTable(QString tableName, QList<PasteListing> *pasteList) {
    if(!pasteDb_.isOpen()) { return; }

    QSqlQuery query(pasteDb_);
    query.prepare("DELETE FROM " + tableName);
    if(!query.exec()) {
        qDebug() << "Unable to truncate table:" << query.lastError();
        return;
    }

    query.prepare("INSERT INTO " + tableName +
            "(key, paste_date, title, size, expire_date, visibility, format_long, format_short, url, hits) "
            "VALUES (:key, :paste_date, :title, :size, :expire_date, :visibility, :format_long, :format_short, :url, :hits)");

    foreach(PasteListing paste, *pasteList) {
        query.bindValue(":key", paste.key());
        query.bindValue(":paste_date", paste.pasteDate().toTime_t());
        query.bindValue(":title", paste.title());
        query.bindValue(":size", paste.pasteSize());
        query.bindValue(":expire_date", paste.expireDate().toTime_t());
        query.bindValue(":visibility", static_cast<int>(paste.visibility()));
        query.bindValue(":format_long", paste.formatLong());
        query.bindValue(":format_short", paste.formatShort());
        query.bindValue(":url", paste.url());
        query.bindValue(":hits", paste.hits());

        if(!query.exec()) {
            qDebug() << "Unable to insert into table:" << query.lastError();
        }
    }
}

void PasteModel::loadPasteTable(QString tableName, QMapListDataModel *dataModel) {
    if(!pasteDb_.isOpen()) { return; }

    QSqlQuery query(pasteDb_);
    query.prepare("SELECT "
            "key, paste_date, title, size, expire_date, visibility, format_long, format_short, url, hits "
            "FROM " + tableName);
    if(!query.exec()) {
        qDebug() << "Unable to query database:" << query.lastError();
        return;
    }

    QList<PasteListing> pasteList;
    while(query.next()) {
        PasteListing paste;
        paste.setKey(query.value(0).toString());
        paste.setPasteDate(QDateTime::fromTime_t(query.value(1).toUInt()));
        paste.setTitle(query.value(2).toString());
        paste.setPasteSize(query.value(3).toInt());
        paste.setExpireDate(QDateTime::fromTime_t(query.value(4).toUInt()));
        paste.setVisibility(static_cast<PasteListing::Visibility>(query.value(5).toInt()));
        paste.setFormatLong(query.value(6).toString());
        paste.setFormatShort(query.value(7).toString());
        paste.setUrl(query.value(8).toString());
        paste.setHits(query.value(9).toInt());
        pasteList.append(paste);
    }

    refreshPasteListing(dataModel, &pasteList);
}
