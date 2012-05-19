#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

#include "Pastebin.h"

#include "config.h"

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
    qDebug() << "Login complete" << statusCode.toInt();

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

QString Pastebin::username() const {
    QSettings settings;
    return settings.value("api_user_name", "").toString();
}

QString Pastebin::apiKey() const {
    QSettings settings;
    return settings.value("api_user_key", "").toString();
}
