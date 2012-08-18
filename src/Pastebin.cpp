#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QNetworkReply>

#include "Pastebin.h"
#include "PasteListing.h"
#include "AppSettings.h"
#include "config.h"

const char* const PASTEBIN_DEV_KEY = PASTEBIN_API_KEY;
const char* const URLENCODED_CONTENT_TYPE = "application/x-www-form-urlencoded";

class PasteUserData {
public:
    PasteUserData() : accountType(AppSettings::Normal), pasteVisibility(PasteListing::Public) { }
    QString username;
    QString avatarUrl;
    QString website;
    QString email;
    QString location;
    AppSettings::AccountType accountType;
    QString pasteFormatShort;
    QString pasteExpiration;
    PasteListing::Visibility pasteVisibility;
};

Pastebin::Pastebin(QObject *parent)
    : QObject(parent) {

}

Pastebin::~Pastebin() {

}

void Pastebin::login(const QString& username, const QString& password) {
    QNetworkRequest request(QUrl("http://pastebin.com/api/api_login.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, URLENCODED_CONTENT_TYPE);

    QUrl params;
    params.addQueryItem("api_dev_key", PASTEBIN_DEV_KEY);
    params.addQueryItem("api_user_name", username.toUtf8());
    params.addQueryItem("api_user_password", password.toUtf8());

    QNetworkReply *reply = accessManager_.post(request, params.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(onLoginFinished()));
}

void Pastebin::onLoginFinished() {
    QNetworkReply *networkReply = qobject_cast<QNetworkReply *>(sender());
    QVariant statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Login complete:" << statusCode.toInt();

    if(networkReply->error() == QNetworkReply::NoError) {
        QString response = networkReply->readAll();

        qDebug() << "Response:" << response;

        if(response.startsWith("Bad API request")) {
            emit loginFailed(response);
        }
        else {
            QNetworkRequest networkRequest = networkReply->request();

            emit loginComplete(response);
        }
    }
    else {
        emit loginFailed("Error");
    }
}

void Pastebin::submitPaste(const QString& pasteContent, const QString& pasteTitle, const QString& format, const QString& expiration, const PasteListing::Visibility visibility) {
    qDebug() << "submitPaste()";

    QNetworkRequest request(QUrl("http://pastebin.com/api/api_post.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, URLENCODED_CONTENT_TYPE);

    QUrl params;
    params.addQueryItem("api_dev_key", PASTEBIN_DEV_KEY);
    params.addQueryItem("api_user_key", apiKey());
    params.addQueryItem("api_option", "paste");
    params.addQueryItem("api_paste_code", pasteContent);
    params.addQueryItem("api_paste_private", QString("%1").arg(static_cast<int>(visibility)));
    if(!pasteTitle.isEmpty()) {
        params.addQueryItem("api_paste_name", pasteTitle);
    }
    if(!expiration.isEmpty()) {
        params.addQueryItem("api_paste_expire_date", expiration);
    }
    if(!format.isEmpty()) {
        params.addQueryItem("api_paste_format", format);
    }

    QNetworkReply *reply = accessManager_.post(request, params.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(onSubmitPasteFinished()));
}

void Pastebin::requestUserDetails() {
    QNetworkRequest request(QUrl("http://pastebin.com/api/api_post.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, URLENCODED_CONTENT_TYPE);

    QUrl params;
    params.addQueryItem("api_dev_key", PASTEBIN_DEV_KEY);
    params.addQueryItem("api_user_key", apiKey());
    params.addQueryItem("api_option", "userdetails");

    QNetworkReply *reply = accessManager_.post(request, params.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(onUserDetailsFinished()));
}

void Pastebin::requestHistory() {
    QNetworkRequest request(QUrl("http://pastebin.com/api/api_post.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, URLENCODED_CONTENT_TYPE);

    QUrl params;
    params.addQueryItem("api_dev_key", PASTEBIN_DEV_KEY);
    params.addQueryItem("api_user_key", apiKey());
    params.addQueryItem("api_option", "list");

    QNetworkReply *reply = accessManager_.post(request, params.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(onHistoryFinished()));
}

void Pastebin::requestTrending() {
    QNetworkRequest request(QUrl("http://pastebin.com/api/api_post.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, URLENCODED_CONTENT_TYPE);

    QUrl params;
    params.addQueryItem("api_dev_key", PASTEBIN_DEV_KEY);
    params.addQueryItem("api_option", "trends");

    QNetworkReply *reply = accessManager_.post(request, params.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(onTrendingFinished()));
}

void Pastebin::onSubmitPasteFinished() {
    QNetworkReply *networkReply = qobject_cast<QNetworkReply *>(sender());
    QVariant statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Paste complete:" << statusCode.toInt();

    if(networkReply->error() == QNetworkReply::NoError) {
        QString response = networkReply->readAll();

        qDebug() << "Response:" << response;

        if(response.startsWith("Bad API request")) {
            qDebug() << "Error with paste";
            emit pasteFailed(response);
        }
        else {
            qDebug() << "Paste successful";
            emit pasteComplete(response);
        }
    }
    else {
        qDebug() << "Error with paste";
        emit pasteFailed("Error");
    }
}

void Pastebin::onUserDetailsFinished() {
    QNetworkReply *networkReply = qobject_cast<QNetworkReply *>(sender());
    QVariant statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "User details request complete:" << statusCode.toInt();

    if(networkReply->error() == QNetworkReply::NoError) {
        QXmlStreamReader reader;
        reader.addData("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n");
        reader.addData("<response>");
        reader.addData(networkReply->readAll());
        reader.addData("</response>");

        bool success = false;

        PasteUserData pasteUser;
        while(!reader.atEnd()) {
            QXmlStreamReader::TokenType token = reader.readNext();
            if(token == QXmlStreamReader::StartDocument) {
                qDebug() << "StartDocument";
                continue;
            }
            else if(token == QXmlStreamReader::StartElement) {
                if(reader.name() == "user") {
                    parseUserDetails(reader, &pasteUser);
                    success = true;
                }
            }
            else if(token == QXmlStreamReader::EndDocument) {
                qDebug() << "EndDocument";
                continue;
            }
        }

        qDebug() << "Parsed user details";

        if(!success || reader.hasError()) {
            qDebug() << "Parse error:" << reader.errorString();
        } else {
            AppSettings *appSettings = AppSettings::instance();
            appSettings->setUsername(pasteUser.username);
            appSettings->setAvatarUrl(pasteUser.avatarUrl);
            appSettings->setWebsite(pasteUser.website);
            appSettings->setEmail(pasteUser.email);
            appSettings->setLocation(pasteUser.location);
            appSettings->setAccountType(pasteUser.accountType);
            appSettings->setPasteFormatShort(pasteUser.pasteFormatShort);
            appSettings->setPasteExpiration(pasteUser.pasteExpiration);
            appSettings->setPasteVisibility(pasteUser.pasteVisibility);
            appSettings->setApiKey(apiKey());
            emit userDetailsUpdated();
        }
    }
    else {
        qDebug() << "Error in user details response";
    }
}

void Pastebin::parseUserDetails(QXmlStreamReader& reader, PasteUserData *pasteUser) {
    qDebug() << "parseUserDetails()";
    while(reader.readNext() != QXmlStreamReader::EndElement) {
        if(reader.name() == "user_name") {
            pasteUser->username = reader.readElementText();
        }
        else if(reader.name() == "user_format_short") {
            pasteUser->pasteFormatShort = reader.readElementText();
        }
        else if(reader.name() == "user_expiration") {
            pasteUser->pasteExpiration = reader.readElementText();
        }
        else if(reader.name() == "user_avatar_url") {
            pasteUser->avatarUrl = reader.readElementText();
        }
        else if(reader.name() == "user_private") {
            int value = reader.readElementText().toInt();
            if(value == 0) {
                pasteUser->pasteVisibility = PasteListing::Public;
            }
            else if(value == 1) {
                pasteUser->pasteVisibility = PasteListing::Unlisted;
            }
            else if(value == 2) {
                pasteUser->pasteVisibility = PasteListing::Private;
            }
        }
        else if(reader.name() == "user_website") {
            pasteUser->website = reader.readElementText();
        }
        else if(reader.name() == "user_email") {
            pasteUser->email = reader.readElementText();
        }
        else if(reader.name() == "user_location") {
            pasteUser->location = reader.readElementText();
        }
        else if(reader.name() == "user_account_type") {
            int value = reader.readElementText().toInt();
            if(value == 0) {
                pasteUser->accountType = AppSettings::Normal;
            }
            else if(value == 1) {
                pasteUser->accountType = AppSettings::Pro;
            }
        }
    }
}

void Pastebin::onHistoryFinished() {
    QNetworkReply *networkReply = qobject_cast<QNetworkReply *>(sender());
    QList<PasteListing> *pasteList = new QList<PasteListing>();
    processPasteListResponse(networkReply, pasteList);
    emit historyAvailable(pasteList);
}

void Pastebin::onTrendingFinished() {
    QNetworkReply *networkReply = qobject_cast<QNetworkReply *>(sender());
    QList<PasteListing> *pasteList = new QList<PasteListing>();
    processPasteListResponse(networkReply, pasteList);
    emit trendingAvailable(pasteList);
}

bool Pastebin::processPasteListResponse(QNetworkReply *networkReply, QList<PasteListing> *pasteList) {
    bool result;
    QVariant statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Paste list request complete:" << statusCode.toInt();

    if(networkReply->error() == QNetworkReply::NoError) {
        QXmlStreamReader reader;
        // Wrap the server response in the necessary bits to make it more
        // closely resemble a valid XML document.  This is necessary because
        // the Pastebin API simply returns an unenclosed list of XML fragments.
        reader.addData("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n");
        reader.addData("<response>");
        reader.addData(networkReply->readAll());
        reader.addData("</response>");

        result = parsePasteList(reader, pasteList);
    }
    else {
        qDebug() << "Error in paste list response";
        result = false;
    }
    return result;
}

bool Pastebin::parsePasteList(QXmlStreamReader& reader, QList<PasteListing> *pasteList) {
    while(!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if(token == QXmlStreamReader::StartDocument) {
            qDebug() << "StartDocument";
            continue;
        }
        else if(token == QXmlStreamReader::StartElement) {
            if(reader.name() == "paste") {
                parsePasteElement(reader, pasteList);
            }
        }
        else if(token == QXmlStreamReader::EndDocument) {
            qDebug() << "EndDocument";
            continue;
        }
    }

    qDebug() << "Parsed" << pasteList->size() << "paste elements";

    if(reader.hasError()) {
        qDebug() << "Parse error:" << reader.errorString();
        return false;
    }
    else {
        return true;
    }
}

void Pastebin::parsePasteElement(QXmlStreamReader& reader, QList<PasteListing> *pasteList) {
    qDebug() << "parsePasteElement()";
    PasteListing paste;
    while(reader.readNext() != QXmlStreamReader::EndElement) {
        if(reader.name() == "paste_key") {
            paste.setKey(reader.readElementText());
        }
        else if(reader.name() == "paste_date") {
            QString dateText = reader.readElementText();
            paste.setPasteDate(QDateTime::fromTime_t(dateText.toLongLong()));
        }
        else if(reader.name() == "paste_title") {
            paste.setTitle(reader.readElementText());
        }
        else if(reader.name() == "paste_size") {
            paste.setPasteSize(reader.readElementText().toInt());
        }
        else if(reader.name() == "paste_expire_date") {
            paste.setExpireDate(QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong()));
        }
        else if(reader.name() == "paste_private") {
            int value = reader.readElementText().toInt();
            if(value == 0) {
                paste.setVisibility(PasteListing::Public);
            }
            else if(value == 1) {
                paste.setVisibility(PasteListing::Unlisted);
            }
            else if(value == 2) {
                paste.setVisibility(PasteListing::Private);
            }
        }
        else if(reader.name() == "paste_format_long") {
            paste.setFormatLong(reader.readElementText());
        }
        else if(reader.name() == "paste_format_short") {
            paste.setFormatShort(reader.readElementText());
        }
        else if(reader.name() == "paste_url") {
            paste.setUrl(reader.readElementText());
        }
        else if(reader.name() == "paste_hits") {
            paste.setHits(reader.readElementText().toInt());
        }
    }
    pasteList->append(paste);
}

void Pastebin::setApiKey(const QString& apiKey) {
    apiKey_ = apiKey;
}

QString Pastebin::apiKey() const {
    return apiKey_;
}
