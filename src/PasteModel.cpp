#include <bb/cascades/QListDataModel>
#include <bb/cascades/XmlDataModel>
#include "PasteModel.h"
#include "AppSettings.h"

PasteModel::PasteModel(QObject *parent)
    : QObject(parent) {
    historyModel_ = new QMapListDataModel();
    historyModel_->setParent(this);
    trendingModel_ = new QMapListDataModel();
    trendingModel_->setParent(this);

    pastebin_.setApiKey(AppSettings::instance()->apiKey());
    loadPasteDatabase();

    loadPasteTable("user_pastes", historyModel_);
    loadPasteTable("trending_pastes", trendingModel_);

    loadFormatterMappings();

    connect(&pastebin_, SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    connect(&pastebin_, SIGNAL(userDetailsUpdated()), this, SLOT(onUserDetailsUpdated()));
    connect(&pastebin_, SIGNAL(userAvatarUpdated()), this, SIGNAL(userAvatarUpdated()));
    connect(&pastebin_, SIGNAL(historyAvailable(QList<PasteListing>*)), this, SLOT(onHistoryAvailable(QList<PasteListing>*)));
    connect(&pastebin_, SIGNAL(trendingAvailable(QList<PasteListing>*)), this, SLOT(onTrendingAvailable(QList<PasteListing>*)));
    connect(&pastebin_, SIGNAL(rawPasteAvailable(QString,QByteArray)), this, SLOT(onRawPasteAvailable(QString,QByteArray)));
    connect(&pastebin_, SIGNAL(deletePasteComplete(QString)), this, SLOT(onDeletePasteComplete(QString)));
    connect(&pastebin_, SIGNAL(deletePasteError(QString,QString)), this, SLOT(onDeletePasteError(QString,QString)));
}

PasteModel::~PasteModel() {
    if(pasteDb_.isOpen()) {
        QSqlDatabase::removeDatabase("data/pasteData.db");
        pasteDb_.removeDatabase("QSQLITE");
        qDebug() << "Database closed";
    }
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

DataModel* PasteModel::historyModel() const {
    return historyModel_;
}

DataModel* PasteModel::trendingModel() const {
    return trendingModel_;
}

PasteListing PasteModel::pasteListing(const QString& pasteKey) const {
    const PasteListing pasteListing = pasteListingMap_[pasteKey];
    if(pasteListing.isNull()) {
        return createFakePasteListing(pasteKey);
    }
    else {
        return pasteListing;
    }
}

QString PasteModel::lexerForFormat(const QString& format) const {
    return formatLexerMap_.value(format);
}

void PasteModel::onLoginComplete(QString apiKey) {
    qDebug() << "PasteModel::onLoginComplete()";
    AppSettings::instance()->setApiKey(apiKey);
    AppSettings::instance()->sync();

    pastebin_.setApiKey(apiKey);
    pastebin_.requestUserDetails();
}

void PasteModel::onUserDetailsUpdated() {
    AppSettings::instance()->sync();
    emit userDetailsUpdated();
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

void PasteModel::logout() {
    AppSettings::instance()->removeUserData();
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
        map["format"] = paste.formatShort();
        map["formatDescription"] = paste.formatLong();

        if(paste.visibility() == PasteListing::Public) {
            map["imageSource"] = "asset:///images/item-paste-public.png";
        } else if(paste.visibility() == PasteListing::Unlisted) {
            map["imageSource"] = "asset:///images/item-paste-unlisted.png";
        } else if(paste.visibility() == PasteListing::Private) {
            map["imageSource"] = "asset:///images/item-paste-private.png";
        }

        map["isPrivate"] = (paste.visibility() == PasteListing::Private);
        map["pasteUrl"] = paste.url();
        map["pasteKey"] = paste.key();

        dataModel->append(map);
        pasteListingMap_[paste.key()] = paste;
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

void PasteModel::requestPaste(const QString& pasteKey) {
    if(rawPasteMap_.contains(pasteKey)) {
        onRawPasteAvailable(pasteKey, rawPasteMap_[pasteKey]);
    }
    else {
        pastebin_.requestRawPaste(pasteKey);
    }
}

void PasteModel::onRawPasteAvailable(QString pasteKey, QByteArray rawPaste) {
    rawPasteMap_[pasteKey] = rawPaste;
    const PasteListing pasteListing = pasteListingMap_[pasteKey];
    if(pasteListing.isNull()) {
        PasteListing fakeListing = createFakePasteListing(pasteKey);
        emit pasteAvailable(fakeListing, rawPaste);
    }
    else {
        emit pasteAvailable(pasteListing, rawPaste);
    }
}

void PasteModel::onRawPasteError(QString pasteKey) {
    const PasteListing pasteListing = pasteListingMap_[pasteKey];
    if(pasteListing.isNull()) {
        PasteListing fakeListing = createFakePasteListing(pasteKey);
        emit pasteError(fakeListing);
    }
    else {
        emit pasteError(pasteListing);
    }
}

PasteListing PasteModel::createFakePasteListing(const QString& pasteKey) {
    PasteListing pasteListing;
    pasteListing.setKey(pasteKey);
    pasteListing.setUrl("http://pastebin.com/" + pasteKey);
    pasteListing.setFormatLong("None");
    pasteListing.setFormatShort("text");
    return pasteListing;
}

void PasteModel::loadFormatterMappings()
{
    XmlDataModel dataModel;
    dataModel.setSource(QUrl("models/paste_formats.xml"));
    int count = dataModel.childCount(QVariantList());

    for(int i = 0; i < count; i++) {
        QVariantMap map = dataModel.data(QVariantList() << i).toMap();
        QString name = map["name"].toString();
        QString lexer = map["lexer"].toString();
        if(!name.isNull() && !lexer.isNull()) {
            formatLexerMap_[name] = lexer;
        }
    }
}

void PasteModel::deletePaste(const QString& pasteKey)
{
    emit historyUpdating();

    pastebin_.requestDeletePaste(pasteKey);
}

void PasteModel::onDeletePasteComplete(QString pasteKey)
{
    for(int i = historyModel_->size(); i >= 0; --i) {
        const QString elementKey = historyModel_->value(i).value("pasteKey").toString();
        if(elementKey == pasteKey) {
            historyModel_->removeAt(i);
        }
    }

    emit historyUpdated();
}

void PasteModel::onDeletePasteError(QString pasteKey, QString message)
{
    Q_UNUSED(pasteKey)
    Q_UNUSED(message)
    //TODO: Notify UI of error
    emit historyUpdated();
}
