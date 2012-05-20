#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/Label>
#include <bb/cascades/TextField>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ListView>

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
            connect(historyPage_, SIGNAL(refreshPage()), this, SLOT(onRefreshHistory()));

            trendingPage_ = navigationPane_->findChild<Page*>("trendingPage");
            trendingPage_->findChild<ActionItem*>("refreshAction")->setEnabled(true);
            connect(trendingPage_, SIGNAL(refreshPage()), this, SLOT(onRefreshTrending()));

            settingsPage_ = navigationPane_->findChild<Page*>("settingsPage");
            connect(settingsPage_, SIGNAL(requestLogin()), this, SLOT(onRequestLogin()));
            connect(settingsPage_, SIGNAL(requestLogout()), this, SLOT(onRequestLogout()));
            connect(settingsPage_, SIGNAL(refreshUserDetails()), this, SLOT(onRefreshUserDetails()));

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

void LogicPasteApp::onRefreshUserDetails() {
    qDebug() << "onRefreshUserDetails()";
    connect(&pastebin_, SIGNAL(userDetailsAvailable(PasteUser)), this, SLOT(onUserDetailsAvailable(PasteUser)));

    pastebin_.requestUserDetails();
}

void LogicPasteApp::onUserDetailsAvailable(PasteUser pasteUser) {
    qDebug() << "onHistoryAvailable()";
    disconnect(&pastebin_, SIGNAL(userDetailsAvailable(PasteUser)), this, SLOT(onUserDetailsAvailable(PasteUser)));

    Label *label;

    label = settingsPage_->findChild<Label*>("websiteLabel");
    if(!pasteUser.website().isEmpty()) {
        label->setText(pasteUser.website());
        label->setVisible(true);
    }

    label = settingsPage_->findChild<Label*>("emailLabel");
    if(!pasteUser.email().isEmpty()) {
        label->setText(pasteUser.email());
        label->setVisible(true);
    }

    label = settingsPage_->findChild<Label*>("locationLabel");
    if(!pasteUser.location().isEmpty()) {
        label->setText(pasteUser.location());
        label->setVisible(true);
    }

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

void LogicPasteApp::onRefreshHistory() {
    qDebug() << "onRefreshHistory()";
    connect(&pastebin_, SIGNAL(historyAvailable(QList<PasteListing> *)), this, SLOT(onHistoryAvailable(QList<PasteListing> *)));

    pastebin_.requestHistory();
}

void LogicPasteApp::onHistoryAvailable(QList<PasteListing> *pasteList) {
    qDebug() << "onHistoryAvailable()";
    disconnect(&pastebin_, SIGNAL(historyAvailable(QList<PasteListing> *)), this, SLOT(onHistoryAvailable(QList<PasteListing> *)));
    refreshPasteListing(historyPage_, &historyModel_, pasteList);
    delete pasteList;
}

void LogicPasteApp::onRefreshTrending() {
    qDebug() << "onRefreshTrending()";
    connect(&pastebin_, SIGNAL(trendingAvailable(QList<PasteListing> *)), this, SLOT(onTrendingAvailable(QList<PasteListing> *)));

    pastebin_.requestTrending();
}

void LogicPasteApp::onTrendingAvailable(QList<PasteListing> *pasteList) {
    qDebug() << "onTrendingAvailable()";
    disconnect(&pastebin_, SIGNAL(trendingAvailable(QList<PasteListing> *)), this, SLOT(onTrendingAvailable(QList<PasteListing> *)));
    refreshPasteListing(trendingPage_, &trendingModel_, pasteList);
    delete pasteList;
}

void LogicPasteApp::refreshPasteListing(Page *page, QMapListDataModel *dataModel, QList<PasteListing> *pasteList) {
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

        dataModel->append(map);
    }

    ListView *listView = page->findChild<ListView*>("pasteList");
    listView->setDataModel(dataModel);

    QMetaObject::invokeMethod(page, "refreshComplete");
}

QString LogicPasteApp::getSettingValue(const QString &keyName) {
    QSettings settings;
    if(settings.value(keyName).isNull()) {
        return "";
    } else {
        return settings.value(keyName).toString();
    }
}
