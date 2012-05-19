#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/TextField>
#include <bb/cascades/ActionItem>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

#include <bps/navigator.h>

#include "LogicPasteApp.h"

#include "config.h"

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
            historyPage_->findChild<ActionItem*>("refreshAction")->setEnabled(!pastebin_.apiKey().isEmpty());

            trendingPage_ = navigationPane_->findChild<Page*>("trendingPage");
            trendingPage_->findChild<ActionItem*>("refreshAction")->setEnabled(true);

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
    connect(&pastebin_, SIGNAL(loginComplete()), this, SLOT(onLoginComplete()));
    connect(&pastebin_, SIGNAL(loginFailed(QString)), this, SLOT(onLoginFailed(QString)));
    pastebin_.login(username, password);
}

void LogicPasteApp::onLoginComplete() {
    disconnect(&pastebin_, SIGNAL(loginComplete()), this, SLOT(onLoginComplete()));
    disconnect(&pastebin_, SIGNAL(loginFailed(QString)), this, SLOT(onLoginFailed(QString)));

    emit settingsUpdated();
    navigationPane_->popAndDelete();
}

void LogicPasteApp::onLoginFailed(QString message) {
    disconnect(&pastebin_, SIGNAL(loginComplete()), this, SLOT(onLoginComplete()));
    disconnect(&pastebin_, SIGNAL(loginFailed(QString)), this, SLOT(onLoginFailed(QString)));

    emit loginFailed(message);
}

void LogicPasteApp::onCreateAccount() {
    qDebug() << "onCreateAccount()";

    navigator_invoke("http://pastebin.com/signup", 0);
}

void LogicPasteApp::onRequestLogout() {
    qDebug() << "onRequestLogout()";

    pastebin_.logout();

    emit settingsUpdated();
}

QString LogicPasteApp::getSettingValue(const QString &keyName) {
    QSettings settings;
    if(settings.value(keyName).isNull()) {
        return "";
    } else {
        return settings.value(keyName).toString();
    }
}
