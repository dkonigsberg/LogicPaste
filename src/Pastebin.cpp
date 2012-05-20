#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QNetworkReply>

#include "Pastebin.h"
#include "config.h"
#include "PasteListing.h"

const char* const PASTEBIN_DEV_KEY = PASTEBIN_API_KEY;
const char* const URLENCODED_CONTENT_TYPE = "application/x-www-form-urlencoded";

Pastebin::Pastebin(QObject *parent)
    : QObject(parent) {

}

Pastebin::~Pastebin() {

}

void Pastebin::login(const QString& username, const QString& password) {
    QNetworkRequest request(QUrl("http://pastebin.com/api/api_login.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, URLENCODED_CONTENT_TYPE);
    request.setAttribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1), username);

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
            QString username = networkRequest.attribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1)).toString();

            QSettings settings;
            settings.setValue("api_user_name", username);
            settings.setValue("api_user_key", response);

            emit loginComplete();
        }
    }
    else {
        emit loginFailed("Error");
    }
}

void Pastebin::logout() {
    QSettings settings;
    settings.remove("api_user_name");
    settings.remove("api_user_key");
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

        PasteUser pasteUser;
        bool success = false;

        while(!reader.atEnd()) {
            QXmlStreamReader::TokenType token = reader.readNext();
            if(token == QXmlStreamReader::StartDocument) {
                qDebug() << "StartDocument";
                continue;
            }
            else if(token == QXmlStreamReader::StartElement) {
                if(reader.name() == "user") {
                    parseUserDetails(reader, pasteUser);
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
            emit userDetailsAvailable(pasteUser);
        }
    }
    else {
        qDebug() << "Error in user details response";
    }
}

void Pastebin::parseUserDetails(QXmlStreamReader& reader, PasteUser& pasteUser) {
    qDebug() << "parseUserDetails()";
    while(reader.readNext() != QXmlStreamReader::EndElement) {
        if(reader.name() == "user_name") {
            pasteUser.setUsername(reader.readElementText());
        }
        else if(reader.name() == "user_format_short") {
            pasteUser.setPasteFormatShort(reader.readElementText());
        }
        else if(reader.name() == "user_expiration") {
            pasteUser.setPasteExpiration(reader.readElementText());
        }
        else if(reader.name() == "user_avatar_url") {
            pasteUser.setAvatarUrl(reader.readElementText());
        }
        else if(reader.name() == "user_private") {
            int value = reader.readElementText().toInt();
            if(value == 0) {
                pasteUser.setPasteVisibility(PasteListing::Public);
            }
            else if(value == 1) {
                pasteUser.setPasteVisibility(PasteListing::Unlisted);
            }
            else if(value == 2) {
                pasteUser.setPasteVisibility(PasteListing::Private);
            }
        }
        else if(reader.name() == "user_website") {
            pasteUser.setWebsite(reader.readElementText());
        }
        else if(reader.name() == "user_email") {
            pasteUser.setEmail(reader.readElementText());
        }
        else if(reader.name() == "user_location") {
            pasteUser.setLocation(reader.readElementText());
        }
        else if(reader.name() == "user_account_type") {
            int value = reader.readElementText().toInt();
            if(value == 0) {
                pasteUser.setAccountType(PasteUser::Normal);
            }
            else if(value == 1) {
                pasteUser.setAccountType(PasteUser::Pro);
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
            paste.setPasteDate(QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong()));
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

QString Pastebin::username() const {
    QSettings settings;
    return settings.value("api_user_name", "").toString();
}

QString Pastebin::apiKey() const {
    QSettings settings;
    return settings.value("api_user_key", "").toString();
}
