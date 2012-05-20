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

void Pastebin::requestTrending() {
    QNetworkRequest request(QUrl("http://pastebin.com/api/api_post.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, URLENCODED_CONTENT_TYPE);

    QUrl params;
    params.addQueryItem("api_dev_key", PASTEBIN_DEV_KEY);
    params.addQueryItem("api_option", "trends");

    QNetworkReply *reply = accessManager_.post(request, params.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(onTrendingFinished()));
}

void Pastebin::onTrendingFinished() {
    QNetworkReply *networkReply = qobject_cast<QNetworkReply *>(sender());
    QVariant statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Trending request complete:" << statusCode.toInt();

    if(networkReply->error() == QNetworkReply::NoError) {

        QList<PasteListing*> *pasteList = new QList<PasteListing*>();;

        QXmlStreamReader reader;
        reader.addData("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
        reader.addData("<response>");
        reader.addData(networkReply->readAll());
        reader.addData("</response>");

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
        }

        qDebug() << "Parsed" << pasteList->size() << "pastes";

        emit trendingAvailable(pasteList);

        if(reader.hasError()) {
            qDebug() << "Parse error:" << reader.errorString();
        }
    }
    else {
        qDebug() << "Error";
    }
}

void Pastebin::parsePasteElement(QXmlStreamReader& reader, QList<PasteListing*> *pasteList) {
    qDebug() << "parsePaste()";
    PasteListing *paste = new PasteListing();
    while(reader.readNext() != QXmlStreamReader::EndElement) {
        if(reader.name() == "paste_key") {
            paste->key_ = reader.readElementText();
        }
        else if(reader.name() == "paste_date") {
            paste->pasteDate_ = QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong());
        }
        else if(reader.name() == "paste_title") {
            paste->title_ = reader.readElementText();
        }
        else if(reader.name() == "paste_size") {
            paste->size_ = reader.readElementText().toInt();
        }
        else if(reader.name() == "paste_expire_date") {
            paste->expireDate_ = QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong());
        }
        else if(reader.name() == "paste_private") {
            int value = reader.readElementText().toInt();
            if(value == 0) {
                paste->visibility_ = PasteListing::Public;
            }
            else if(value == 1) {
                paste->visibility_ = PasteListing::Unlisted;
            }
            else if(value == 2) {
                paste->visibility_ = PasteListing::Private;
            }
        }
        else if(reader.name() == "paste_format_long") {
            paste->formatLong_ = reader.readElementText();
        }
        else if(reader.name() == "paste_format_short") {
            paste->formatShort_ = reader.readElementText();
        }
        else if(reader.name() == "paste_url") {
            paste->url_ = reader.readElementText();
        }
        else if(reader.name() == "paste_hits") {
            paste->hits_ = reader.readElementText().toInt();
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
