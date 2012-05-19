#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/TextField>
#include <bb/cascades/Button>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

#include <bps/navigator.h>

#include "LogicPasteApp.h"

#include "config.h"

const char* const LogicPasteApp::pasteBinLoginUrl_ = "http://pastebin.com/api/api_login.php";
const char* const LogicPasteApp::pasteBinDevKey_ = PASTEBIN_API_KEY;

LogicPasteApp::LogicPasteApp() {
    QCoreApplication::setOrganizationName("LogicProbe");
    QCoreApplication::setApplicationName("LogicPaste");

    QmlDocument *qml = QmlDocument::create(this, "main.qml");
    qml->setContextProperty("cs", this);

    if(!qml->hasErrors()) {
        navigationPane_ = qml->createRootNode<NavigationPane>();
        if(navigationPane_) {
            pastePage_ = navigationPane_->findChild<Page*>("pastePage");

            historyPage_ = navigationPane_->findChild<Page*>("historyPage");

            settingsPage_ = navigationPane_->findChild<Page*>("settingsPage");
            connect(settingsPage_, SIGNAL(requestLogin()), this, SLOT(onRequestLogin()));
            connect(settingsPage_, SIGNAL(requestLogout()), this, SLOT(onRequestLogout()));

            Application::setScene(navigationPane_);
        }
    }
}

void LogicPasteApp::onRequestLogin() {
    qDebug() << "onRequestLogin()";

    QmlDocument *qml = QmlDocument::create(this, "LoginPage.qml");
    if(!qml->hasErrors()) {
        Page *loginPage = qml->createRootNode<Page>();
        qml->setContextProperty("cs", this);
        connect(loginPage, SIGNAL(processLogin(QString,QString)), this, SLOT(onProcessLogin(QString,QString)));
        connect(loginPage, SIGNAL(createAccount()), this, SLOT(onCreateAccount()));
        navigationPane_->push(loginPage);
    }
}

void LogicPasteApp::onProcessLogin(QString username, QString password) {
    qDebug() << "onProcessLogin" << username << "," << password;

    QNetworkRequest request(QUrl(LogicPasteApp::pasteBinLoginUrl_));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setAttribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1), username);

    QUrl params;
    params.addQueryItem("api_dev_key", LogicPasteApp::pasteBinDevKey_);
    params.addQueryItem("api_user_name", username.toUtf8());
    params.addQueryItem("api_user_password", password.toUtf8());

    QNetworkReply *reply = accessManager_.post(request, params.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(onPasteBinApiLogin()));
}

void LogicPasteApp::onCreateAccount() {
    qDebug() << "onCreateAccount()";

    navigator_invoke("http://pastebin.com/signup", 0);
}

void LogicPasteApp::onRequestLogout() {
    qDebug() << "onRequestLogout()";

    QSettings settings;
    settings.remove("api_user_name");
    settings.remove("api_user_key");
    emit settingsUpdated();
}

void LogicPasteApp::onPasteBinApiLogin() {
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
            emit settingsUpdated();
            navigationPane_->popAndDelete();
        }
    }
}

QString LogicPasteApp::getSettingValue(const QString &keyName) {
    QSettings settings;
    if(settings.value(keyName).isNull()) {
        return "";
    } else {
        return settings.value(keyName).toString();
    }
}
