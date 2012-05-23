#include <bb/cascades/QListDataModel>
#include "PasteModel.h"

PasteModel::PasteModel(QObject *parent)
    : QObject(parent) {
    historyModel_ = new QMapListDataModel();
    trendingModel_ = new QMapListDataModel();

    connect(&pastebin_, SIGNAL(userDetailsAvailable(PasteUser)), this, SLOT(onUserDetailsAvailable(PasteUser)));
    connect(&pastebin_, SIGNAL(historyAvailable(QList<PasteListing> *)), this, SLOT(onHistoryAvailable(QList<PasteListing> *)));
    connect(&pastebin_, SIGNAL(trendingAvailable(QList<PasteListing> *)), this, SLOT(onTrendingAvailable(QList<PasteListing> *)));
}

PasteModel::~PasteModel() {
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

QString PasteModel::username() const {
    return pastebin_.username();
}

QString PasteModel::apiKey() const {
    return pastebin_.apiKey();
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

void PasteModel::onUserDetailsAvailable(PasteUser pasteUser) {
    pasteUser_ = pasteUser;
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
