#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/Page>
#include <bb/cascades/Sheet>
#include <bb/cascades/Label>
#include <bb/cascades/TextField>
#include <bb/cascades/TextArea>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ListView>
#include <bb/cascades/DropDown>
#include <bb/cascades/Container>
#include <bb/cascades/WebView>
#include <bb/cascades/WebSettings>
#include <bb/cascades/ImageView>
#include <bb/cascades/PixelBufferData>
#include <bb/cascades/Image>
#include <bb/system/SystemDialog>
#include <bb/system/Clipboard>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

#include <bps/navigator.h>

#include "FormatDropDown.h"
#include "LogicPasteApp.h"
#include "AppSettings.h"

#include "config.h"

LogicPasteApp::LogicPasteApp() : loginSheet_(NULL) {
    QCoreApplication::setOrganizationName("LogicProbe");
    QCoreApplication::setApplicationName("LogicPaste");

    pasteModel_ = new PasteModel(this);

    QmlDocument *qml = QmlDocument::create(this, "main.qml");
    qml->setContextProperty("cs", this);
    qml->setContextProperty("model", pasteModel_);

    if(!qml->hasErrors()) {
        TabbedPane *tabbedPane = qml->createRootNode<TabbedPane>();
        if(tabbedPane) {
            // Paste page
            pastePage_ = tabbedPane->findChild<Page*>("pastePage");
            connect(pastePage_, SIGNAL(submitPaste()), this, SLOT(onSubmitPaste()));
            replaceDropDown(pastePage_, "formatDropDown");

            // History page
            historyNav_ = tabbedPane->findChild<NavigationPane*>("historyPage");
            historyPage_ = historyNav_->findChild<Page*>("pasteListPage");
            historyPage_->findChild<ActionItem*>("refreshAction")->setEnabled(pasteModel_->isAuthenticated());
            connect(historyPage_, SIGNAL(refreshPage()), pasteModel_, SLOT(refreshHistory()));

            ListView *historyList = historyPage_->findChild<ListView*>("pasteList");
            historyList->setDataModel(pasteModel_->historyModel());
            connect(historyList, SIGNAL(openPaste(QString, QString)), this, SLOT(onOpenHistoryPaste(QString, QString)));
            connect(historyList, SIGNAL(openPasteInBrowser(QString)), this, SLOT(onOpenPasteInBrowser(QString)));
            connect(historyList, SIGNAL(copyUrl(QString)), this, SLOT(onCopyText(QString)));

            connect(pasteModel_, SIGNAL(historyUpdating()), historyPage_, SLOT(onRefreshStarted()));
            connect(pasteModel_, SIGNAL(historyUpdated()), historyPage_, SLOT(onRefreshComplete()));

            // Trending page
            trendingNav_ = tabbedPane->findChild<NavigationPane*>("trendingPage");
            trendingPage_ = trendingNav_->findChild<Page*>("pasteListPage");
            trendingPage_->findChild<ActionItem*>("refreshAction")->setEnabled(true);
            connect(trendingPage_, SIGNAL(refreshPage()), pasteModel_, SLOT(refreshTrending()));

            ListView *trendingList = trendingPage_->findChild<ListView*>("pasteList");
            trendingList->setDataModel(pasteModel_->trendingModel());
            connect(trendingList, SIGNAL(openPaste(QString, QString)), this, SLOT(onOpenTrendingPaste(QString, QString)));
            connect(trendingList, SIGNAL(openPasteInBrowser(QString)), this, SLOT(onOpenPasteInBrowser(QString)));
            connect(trendingList, SIGNAL(copyUrl(QString)), this, SLOT(onCopyText(QString)));

            connect(pasteModel_, SIGNAL(trendingUpdating()), trendingPage_, SLOT(onRefreshStarted()));
            connect(pasteModel_, SIGNAL(trendingUpdated()), trendingPage_, SLOT(onRefreshComplete()));

            // Settings page
            settingsPage_ = tabbedPane->findChild<Page*>("settingsPage");
            connect(settingsPage_, SIGNAL(requestLogin()), this, SLOT(onRequestLogin()));
            connect(settingsPage_, SIGNAL(requestLogout()), this, SLOT(onRequestLogout()));
            connect(settingsPage_, SIGNAL(refreshUserDetails()), pasteModel_, SLOT(refreshUserDetails()));
            connect(pasteModel_, SIGNAL(userDetailsUpdated()), this, SLOT(onUserDetailsUpdated()));
            connect(pasteModel_, SIGNAL(userAvatarUpdated()), this, SLOT(onUserAvatarUpdated()));
            replaceDropDown(settingsPage_, "formatDropDown");

            // Tabbed pane
            connect(tabbedPane, SIGNAL(activePaneChanged(bb::cascades::AbstractPane*)),
                this, SLOT(onActivePaneChanged(bb::cascades::AbstractPane*)));

            connect(pasteModel_->pastebin(), SIGNAL(formattedPasteAvailable(QString, QString)),
                this, SLOT(onFormattedPasteAvailable(QString, QString)));

            Application::setScene(tabbedPane);

            if(pasteModel_->isAuthenticated()) {
                onUserDetailsUpdated();
                onUserAvatarUpdated();
            }
        }
    }
}

FormatDropDown* LogicPasteApp::replaceDropDown(Page *page, const QString& objectName)
{
    DropDown *dropDown = page->findChild<DropDown*>(objectName);
    if(!dropDown) { return NULL; }

    Container *container = qobject_cast<Container*>(dropDown->parent());
    if(!container) { return NULL; }

    int index = container->indexOf(dropDown);

    FormatDropDown *formatDropDown = new FormatDropDown();
    formatDropDown->setObjectName(objectName);
    formatDropDown->setTitle(dropDown->title());
    container->replace(index, formatDropDown);

    return formatDropDown;
}

void LogicPasteApp::onRequestLogin() {
    qDebug() << "onRequestLogin()";

    if(loginSheet_) {
        delete loginSheet_;
        loginSheet_ = NULL;
    }

    QmlDocument *qml = QmlDocument::create(this, "LoginPage.qml");
    if(!qml->hasErrors()) {
        Page *loginPage = qml->createRootNode<Page>();
        loginPage->setResizeBehavior(PageResizeBehavior::None);
        qml->setContextProperty("cs", this);

        connect(loginPage, SIGNAL(processLogin(QString,QString)), this, SLOT(onProcessLogin(QString,QString)));
        connect(loginPage, SIGNAL(createAccount()), this, SLOT(onCreateAccount()));
        connect(loginPage, SIGNAL(cancel()), this, SLOT(onLoginCanceled()));

        loginSheet_ = Sheet::create();
        loginSheet_->setParent(this);
        loginSheet_->setContent(loginPage);
        loginSheet_->setVisible(true);
    }
}

void LogicPasteApp::onProcessLogin(QString username, QString password) {
    qDebug() << "onProcessLogin" << username << "," << password;
    connect(pasteModel_->pastebin(), SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    connect(pasteModel_->pastebin(), SIGNAL(loginFailed(QString)), this, SLOT(onLoginFailed(QString)));
    pasteModel_->pastebin()->login(username, password);
}

void LogicPasteApp::onLoginComplete(QString apiKey) {
    qDebug() << "onLoginComplete()";
    disconnect(pasteModel_->pastebin(), SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    disconnect(pasteModel_->pastebin(), SIGNAL(loginFailed(QString)), this, SLOT(onLoginFailed(QString)));

    Label *label = settingsPage_->findChild<Label*>("keyLabel");
    label->setText(QString("API Key: %1").arg(apiKey));
    label->setVisible(true);

    emit settingsUpdated();
    historyPage_->findChild<ActionItem*>("refreshAction")->setEnabled(pasteModel_->isAuthenticated());

    loginSheet_->setVisible(false);
    loginSheet_->deleteLater();
    loginSheet_ = NULL;
}

void LogicPasteApp::onLoginFailed(QString message) {
    qDebug() << "onLoginFailed()";
    disconnect(pasteModel_->pastebin(), SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    disconnect(pasteModel_->pastebin(), SIGNAL(loginFailed(QString)), this, SLOT(onLoginFailed(QString)));

    emit loginFailed(message);
}

void LogicPasteApp::onLoginCanceled() {
    qDebug() << "onLoginCanceled()";
    if(loginSheet_) {
        loginSheet_->setVisible(false);
        loginSheet_->deleteLater();
        loginSheet_ = NULL;
    }
}

void LogicPasteApp::onUserDetailsUpdated() {
    qDebug() << "onUserDetailsUpdated()";

    AppSettings *appSettings = AppSettings::instance();
    Label *label;

    if(!appSettings->username().isEmpty()) {
        label = settingsPage_->findChild<Label*>("userLabel");
        label->setText(QString(tr("Username: %1")).arg(appSettings->username()));
        label->setVisible(true);
    }

    if(pasteModel_->isAuthenticated()) {
        label = settingsPage_->findChild<Label*>("keyLabel");
        label->setText(QString(tr("API Key: %1")).arg(appSettings->apiKey()));
        label->setVisible(true);
    }

    if(!appSettings->website().isEmpty()) {
        label = settingsPage_->findChild<Label*>("websiteLabel");
        label->setText(QString(tr("Website: %1")).arg(appSettings->website()));
        label->setVisible(true);
    }

    if(!appSettings->email().isEmpty()) {
        label = settingsPage_->findChild<Label*>("emailLabel");
        label->setText(QString(tr("Email: %1")).arg(appSettings->email()));
        label->setVisible(true);
    }

    if(!appSettings->location().isEmpty()) {
        label = settingsPage_->findChild<Label*>("locationLabel");
        label->setText(QString(tr("Location: %1")).arg(appSettings->location()));
        label->setVisible(true);
    }

    FormatDropDown *formatDropDown;
    if(!appSettings->pasteFormatShort().isEmpty()) {
        formatDropDown = settingsPage_->findChild<FormatDropDown*>("formatDropDown");
        if(formatDropDown) {
            formatDropDown->selectFormat(appSettings->pasteFormatShort());
        }
    }

    DropDown *dropDown;
    if(!appSettings->pasteExpiration().isEmpty()) {
        dropDown = settingsPage_->findChild<DropDown*>("expirationDropDown");
        if(dropDown) {
            for(int i = dropDown->optionCount() - 1; i >= 0; --i) {
                if(dropDown->at(i)->value() == appSettings->pasteExpiration()) {
                    dropDown->setSelectedIndex(i);
                    break;
                }
            }
        }
    }

    int visibilityValue = static_cast<int>(appSettings->pasteVisibility());
    dropDown = settingsPage_->findChild<DropDown*>("exposureDropDown");
    if(dropDown) {
        dropDown->setSelectedIndex(visibilityValue);
    }

    QMetaObject::invokeMethod(settingsPage_, "userDetailsRefreshed");
}

void LogicPasteApp::onUserAvatarUpdated()
{
    qDebug() << "onUserAvatarUpdated()";
    ImageView *imageView = settingsPage_->findChild<ImageView*>("avatarImage");
    AppSettings *appSettings = AppSettings::instance();
    const QByteArray data = appSettings->avatarImage();
    if(!data.isEmpty()) {
        QImage image = QImage::fromData(data);
        QImage convertedImage = image.rgbSwapped();

        bb::cascades::PixelBufferData pixelData(
            bb::cascades::PixelBufferData::RGBA_PRE,
            image.width(),
            image.height(),
            image.width(),
            (void *)convertedImage.bits());
        Image displayableImage(pixelData);

        imageView->setImage(displayableImage);
        imageView->setPreferredWidth(image.width() * 4);
        imageView->setPreferredHeight(image.height() * 4);
        imageView->setVisible(true);
    }
    else {
        imageView->setVisible(false);
    }
}

void LogicPasteApp::onCreateAccount() {
    qDebug() << "onCreateAccount()";

    navigator_invoke("http://pastebin.com/signup", 0);
}

void LogicPasteApp::onRequestLogout() {
    qDebug() << "onRequestLogout()";

    pasteModel_->logout();

    emit settingsUpdated();
    historyPage_->findChild<ActionItem*>("refreshAction")->setEnabled(pasteModel_->isAuthenticated());
}

void LogicPasteApp::onSubmitPaste() {
    qDebug() << "onSubmitPaste()";

    QString pasteContent = pastePage_->findChild<TextArea*>("pasteTextField")->text();
    QString pasteTitle = pastePage_->findChild<TextField*>("pasteTitleField")->text();

    DropDown *expirationDropDown = pastePage_->findChild<DropDown*>("expirationDropDown");
    QString expiration = expirationDropDown->at(expirationDropDown->selectedIndex())->value().toString();

    FormatDropDown *formatDropDown = pastePage_->findChild<FormatDropDown*>("formatDropDown");
    QString format = formatDropDown->selectedFormat();

    DropDown *exposureDropDown = pastePage_->findChild<DropDown*>("exposureDropDown");
    int value = exposureDropDown->at(exposureDropDown->selectedIndex())->value().toInt();
    PasteListing::Visibility visibility;
    if(value == 0) {
        visibility = PasteListing::Public;
    } else if(value == 1) {
        visibility = PasteListing::Unlisted;
    } else {
        visibility = PasteListing::Private;
    }

    connect(pasteModel_->pastebin(), SIGNAL(pasteComplete(QString)), this, SLOT(onPasteComplete(QString)));
    connect(pasteModel_->pastebin(), SIGNAL(pasteFailed(QString)), this, SLOT(onPasteFailed(QString)));

    pasteModel_->pastebin()->submitPaste(pasteContent, pasteTitle, format, expiration, visibility);
}

void LogicPasteApp::onPasteComplete(QString pasteUrl) {
    Q_UNUSED(pasteUrl)
    qDebug() << "onPasteComplete()";
    disconnect(pasteModel_->pastebin(), SIGNAL(pasteComplete(QString)), this, SLOT(onPasteComplete(QString)));
    disconnect(pasteModel_->pastebin(), SIGNAL(pasteFailed(QString)), this, SLOT(onPasteFailed(QString)));

    bb::system::SystemDialog *dialog = new bb::system::SystemDialog(tr("Okay"));
    dialog->setBody(tr("Paste successful"));
    dialog->show();

    QMetaObject::invokeMethod(pastePage_, "pasteSuccess");
}

void LogicPasteApp::onPasteFailed(QString message) {
    Q_UNUSED(message)
    qDebug() << "onPasteFailed()";
    disconnect(pasteModel_->pastebin(), SIGNAL(pasteComplete(QString)), this, SLOT(onPasteComplete(QString)));
    disconnect(pasteModel_->pastebin(), SIGNAL(pasteFailed(QString)), this, SLOT(onPasteFailed(QString)));

    bb::system::SystemDialog *dialog = new bb::system::SystemDialog(tr("Okay"));
    dialog->setBody(tr("Paste failed"));
    dialog->show();

    QMetaObject::invokeMethod(pastePage_, "pasteFailed");
}

void LogicPasteApp::onOpenHistoryPaste(QString pasteUrl, QString format) {
    openPaste(historyNav_, pasteUrl, format);
}

void LogicPasteApp::onOpenTrendingPaste(QString pasteUrl, QString format) {
    openPaste(trendingNav_, pasteUrl, format);
}

void LogicPasteApp::openPaste(NavigationPane *nav, QString pasteUrl, QString format) {
    qDebug().nospace() << "onOpenPaste(" << pasteUrl << ", " << format << ")";

    QmlDocument *qml = QmlDocument::create(this, "ViewPastePage.qml");
    if(!qml->hasErrors()) {
        Page *page = qml->createRootNode<Page>();
        qml->setContextProperty("cs", this);

        pasteModel_->pastebin()->requestFormattedPaste(pasteUrl, format);

        nav->push(page);
    }
}

void LogicPasteApp::onFormattedPasteAvailable(QString pasteUrl, QString html) {
    qDebug().nospace() << "onFormattedPasteAvailable(" << pasteUrl << ")";
    Page *page = historyNav_->top();
    WebView *webView = page->findChild<WebView*>("webView");
    webView->settings()->setMinimumFontSize(36);
    webView->setHtml(html);
}

void LogicPasteApp::onOpenPasteInBrowser(QString pasteUrl) {
    qDebug() << "onOpenPasteInBrowser():" << pasteUrl;

    navigator_invoke(pasteUrl.toLatin1(), 0);
}

void LogicPasteApp::onCopyText(QString text) {
    qDebug() << "onCopyText()";

    bb::system::Clipboard clipboard;
    if(clipboard.clear()) {
        clipboard.insert("text/plain", text.toUtf8());
    }
}

void LogicPasteApp::onActivePaneChanged(bb::cascades::AbstractPane *activePane)
{
    if(activePane == pastePage_ || activePane == settingsPage_) {
        FormatDropDown *dropDown = activePane->findChild<FormatDropDown*>("formatDropDown");
        if(dropDown) {
            dropDown->refreshRecentFormats();
        }
    }
}
