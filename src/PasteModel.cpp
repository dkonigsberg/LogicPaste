#include <bb/cascades/QListDataModel>
#include "PasteModel.h"

PasteModel::PasteModel(QObject *parent)
    : QObject(parent) {
    historyModel_ = new QMapListDataModel();
    trendingModel_ = new QMapListDataModel();

    loadUserDetails();

    connect(&pastebin_, SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    connect(&pastebin_, SIGNAL(userDetailsAvailable(PasteUser)), this, SLOT(onUserDetailsAvailable(PasteUser)));
    connect(&pastebin_, SIGNAL(historyAvailable(QList<PasteListing> *)), this, SLOT(onHistoryAvailable(QList<PasteListing> *)));
    connect(&pastebin_, SIGNAL(trendingAvailable(QList<PasteListing> *)), this, SLOT(onTrendingAvailable(QList<PasteListing> *)));
}

PasteModel::~PasteModel() {
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
    delete pasteList;
    emit historyUpdated();
}

void PasteModel::onTrendingAvailable(QList<PasteListing> *pasteList) {
    qDebug() << "PasteModel::onTrendingAvailable()";
    refreshPasteListing(trendingModel_, pasteList);
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

        dataModel->append(map);
    }
}
