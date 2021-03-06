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
#include <bb/cascades/Menu>
#include <bb/cascades/ListView>
#include <bb/cascades/DropDown>
#include <bb/cascades/CheckBox>
#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Image>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>
#include <bb/system/Clipboard>
#include <bb/ApplicationInfo>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

#include <bps/navigator.h>

#include "FormatDropDown.h"
#include "LogicPasteApp.h"
#include "AppSettings.h"
#include "ViewPastePage.h"
#include "PasteListing.h"

#include "config.h"

LogicPasteApp::LogicPasteApp(Application *app)
    : QObject(app), loginSheet_(NULL), ignoreSettingsEvent_(false) {
    qDebug() << "LogicPasteApp::LogicPasteApp()";
    QCoreApplication::setOrganizationName("LogicProbe");
    QCoreApplication::setApplicationName("LogicPaste");

    qRegisterMetaType<PasteListing>("PasteListing");

    pasteModel_ = new PasteModel(this);

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("cs", this);
    qml->setContextProperty("model", pasteModel_);

    AppSettings *appSettings = AppSettings::instance();

    if(!qml->hasErrors()) {
        tabbedPane_ = qml->createRootObject<TabbedPane>();
        if(tabbedPane_) {
            // Paste page
            pastePage_ = tabbedPane_->findChild<Page*>("pastePage");
            connect(pastePage_, SIGNAL(submitPaste()), this, SLOT(onSubmitPaste()));
            replaceDropDown(pastePage_, "formatDropDown");

            // History page
            historyNav_ = tabbedPane_->findChild<NavigationPane*>("historyPage");
            connect(historyNav_, SIGNAL(popTransitionEnded(bb::cascades::Page*)),
                this, SLOT(onPopFinished(bb::cascades::Page*)));
            historyPage_ = historyNav_->findChild<Page*>("pasteListPage");
            connect(historyPage_, SIGNAL(refreshPage()), pasteModel_, SLOT(refreshHistory()));

            ListView *historyList = historyPage_->findChild<ListView*>("pasteList");
            historyList->setDataModel(pasteModel_->historyModel());
            connect(historyList, SIGNAL(openPaste(QString)), this, SLOT(onOpenHistoryPaste(QString)));
            connect(historyList, SIGNAL(copyUrl(QString)), this, SLOT(onCopyText(QString)));
            connect(historyList, SIGNAL(deletePaste(QString)), this, SLOT(onDeleteHistoryPaste(QString)));

            connect(pasteModel_, SIGNAL(historyUpdating()), historyPage_, SLOT(onRefreshStarted()));
            connect(pasteModel_, SIGNAL(historyUpdated(bool)), this, SLOT(onHistoryRefreshComplete(bool)));

            // Trending page
            trendingNav_ = tabbedPane_->findChild<NavigationPane*>("trendingPage");
            connect(trendingNav_, SIGNAL(popTransitionEnded(bb::cascades::Page*)),
                this, SLOT(onPopFinished(bb::cascades::Page*)));
            trendingPage_ = trendingNav_->findChild<Page*>("pasteListPage");
            trendingPage_->findChild<ActionItem*>("refreshAction")->setEnabled(true);
            connect(trendingPage_, SIGNAL(refreshPage()), pasteModel_, SLOT(refreshTrending()));

            ListView *trendingList = trendingPage_->findChild<ListView*>("pasteList");
            trendingList->setDataModel(pasteModel_->trendingModel());
            connect(trendingList, SIGNAL(openPaste(QString)), this, SLOT(onOpenTrendingPaste(QString)));
            connect(trendingList, SIGNAL(copyUrl(QString)), this, SLOT(onCopyText(QString)));

            connect(pasteModel_, SIGNAL(trendingUpdating()), trendingPage_, SLOT(onRefreshStarted()));
            connect(pasteModel_, SIGNAL(trendingUpdated(bool)), this, SLOT(onTrendingRefreshComplete(bool)));

            // Settings page
            settingsPage_ = tabbedPane_->findChild<Page*>("settingsPage");
            CheckBox *sslCheckBox = settingsPage_->findChild<CheckBox *>("sslCheckBox");
            sslCheckBox->setChecked(appSettings->useSsl());

            CheckBox *formatterEnable = settingsPage_->findChild<CheckBox*>("formatterEnable");
            formatterEnable->setChecked(appSettings->formatterEnabled());

            CheckBox *formatterLineNumbering = settingsPage_->findChild<CheckBox*>("formatterLineNumbering");
            formatterLineNumbering->setChecked(appSettings->formatterLineNumbering());

            DropDown *formatterStyle = settingsPage_->findChild<DropDown*>("formatterStyle");
            for(int i = formatterStyle->count() - 1; i >= 0; --i) {
                if(formatterStyle->at(i)->value() == appSettings->formatterStyle()) {
                    formatterStyle->setSelectedIndex(i);
                    break;
                }
            }

            connect(settingsPage_, SIGNAL(requestLogin()), this, SLOT(onRequestLogin()));
            connect(settingsPage_, SIGNAL(requestLogout()), this, SLOT(onRequestLogout()));
            connect(settingsPage_, SIGNAL(refreshUserDetails()), pasteModel_, SLOT(refreshUserDetails()));
            connect(settingsPage_, SIGNAL(connectionSettingsChanged()), this, SLOT(onConnectionSettingsChanged()));
            connect(settingsPage_, SIGNAL(pasteSettingsChanged()), this, SLOT(onPasteSettingsChanged()));
            connect(settingsPage_, SIGNAL(formatterSettingsChanged()), this, SLOT(onFormatterSettingsChanged()));
            connect(pasteModel_, SIGNAL(userDetailsUpdated()), this, SLOT(onUserDetailsUpdated()));
            connect(pasteModel_, SIGNAL(userDetailsError(QString)), this, SLOT(onUserDetailsError(QString)));
            connect(pasteModel_, SIGNAL(userAvatarUpdated()), this, SLOT(onUserAvatarUpdated()));
            connect(pasteModel_, SIGNAL(deletePasteError(PasteListing,QString)), this, SLOT(onDeletePasteError(PasteListing,QString)));

            FormatDropDown *formatDropDown = replaceDropDown(settingsPage_, "formatDropDown");
            connect(formatDropDown, SIGNAL(selectedIndexChanged(int)), this, SLOT(onPasteSettingsChanged()));

            // Tabbed pane
            connect(tabbedPane_, SIGNAL(activePaneChanged(bb::cascades::AbstractPane*)),
                this, SLOT(onActivePaneChanged(bb::cascades::AbstractPane*)));

            app->setScene(tabbedPane_);

            // Create the pull-down menu
            ActionItem *aboutItem = ActionItem::create()
            .title(tr("About"))
            .image(QUrl("asset:///images/action-about.png"));
            connect(aboutItem, SIGNAL(triggered()), this, SLOT(onAboutActionTriggered()));

            Menu *menu = Menu::create()
            .addAction(aboutItem);
            app->setMenu(menu);

            if(pasteModel_->isAuthenticated()) {
                onUserDetailsUpdated();
                onUserAvatarUpdated();
            }

            refreshPastePageDefaults();
            refreshMainActions();
        }
    }
}

LogicPasteApp::~LogicPasteApp()
{
    qDebug() << "LogicPasteApp::~LogicPasteApp()";
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

void LogicPasteApp::onPopFinished(bb::cascades::Page *page)
{
    if(page) {
        delete page;
    }
}

void LogicPasteApp::onAboutActionTriggered()
{
    QmlDocument *qml = QmlDocument::create("asset:///AboutPage.qml").parent(this);
    qml->setContextProperty("cs", this);

    if(!qml->hasErrors()) {
        Page *aboutPage = qml->createRootObject<Page>();

        bb::ApplicationInfo appInfo;
        aboutPage->setProperty("appName", appInfo.title());
        aboutPage->setProperty("versionNumber", appInfo.version());

        Sheet *sheet = Sheet::create().content(aboutPage);
        connect(aboutPage, SIGNAL(close()), this, SLOT(onSheetPageClosed()));
        connect(aboutPage, SIGNAL(openUrl(QString)), this, SLOT(onOpenUrlInBrowser(QString)));
        sheet->open();
        Application::instance()->setMenuEnabled(false);
    }
}

void LogicPasteApp::onSheetPageClosed()
{
    Page *page = qobject_cast<Page*>(sender());
    Sheet *sheet = qobject_cast<Sheet*>(page->parent());
    sheet->close();
    sheet->deleteLater();
    Application::instance()->setMenuEnabled(true);
}

void LogicPasteApp::onRequestLogin() {
    qDebug() << "onRequestLogin()";

    if(loginSheet_) {
        delete loginSheet_;
        loginSheet_ = NULL;
    }

    QmlDocument *qml = QmlDocument::create("asset:///LoginPage.qml").parent(this);
    if(!qml->hasErrors()) {
        Page *loginPage = qml->createRootObject<Page>();
        loginPage->setResizeBehavior(PageResizeBehavior::None);
        qml->setContextProperty("cs", this);

        connect(loginPage, SIGNAL(processLogin(QString,QString)), this, SLOT(onProcessLogin(QString,QString)));
        connect(loginPage, SIGNAL(createAccount()), this, SLOT(onCreateAccount()));
        connect(loginPage, SIGNAL(cancel()), this, SLOT(onLoginCanceled()));

        loginSheet_ = Sheet::create();
        loginSheet_->setParent(this);
        loginSheet_->setContent(loginPage);
        loginSheet_->open();
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
    refreshMainActions();

    loginSheet_->close();
    loginSheet_->deleteLater();
    loginSheet_ = NULL;
}

void LogicPasteApp::onLoginFailed(QString message) {
    qDebug() << "onLoginFailed()";
    disconnect(pasteModel_->pastebin(), SIGNAL(loginComplete(QString)), this, SLOT(onLoginComplete(QString)));
    disconnect(pasteModel_->pastebin(), SIGNAL(loginFailed(QString)), this, SLOT(onLoginFailed(QString)));

    if(message == "Bad API request, invalid login") {
        emit loginFailed(tr("Unable to login. Please check your username and password."));
    }
    else if(message == "Bad API request, account not active") {
        emit loginFailed(tr("Unable to login. Your Pastebin account is not active."));
    }
    else {
        emit loginFailed(message);
    }
}

void LogicPasteApp::onLoginCanceled() {
    qDebug() << "onLoginCanceled()";
    if(loginSheet_) {
        loginSheet_->close();
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
        label->setText(tr("Username: %1").arg(appSettings->username()));
        label->setVisible(true);
    }

    if(pasteModel_->isAuthenticated()) {
        label = settingsPage_->findChild<Label*>("keyLabel");
        label->setText(tr("API Key: %1").arg(appSettings->apiKey()));
        label->setVisible(true);
    }

    if(!appSettings->website().isEmpty()) {
        label = settingsPage_->findChild<Label*>("websiteLabel");
        label->setText(tr("Website: %1").arg(appSettings->website()));
        label->setVisible(true);
    }

    if(!appSettings->email().isEmpty()) {
        label = settingsPage_->findChild<Label*>("emailLabel");
        label->setText(tr("Email: %1").arg(appSettings->email()));
        label->setVisible(true);
    }

    if(!appSettings->location().isEmpty()) {
        label = settingsPage_->findChild<Label*>("locationLabel");
        label->setText(tr("Location: %1").arg(appSettings->location()));
        label->setVisible(true);
    }

    label = settingsPage_->findChild<Label*>("accountTypeLabel");
    QString accountTypeText;
    switch(appSettings->accountType()) {
    case AppSettings::Normal:
        accountTypeText = tr("Normal");
        break;
    case AppSettings::Pro:
        accountTypeText = tr("Pro");
        break;
    default:
        accountTypeText = tr("Unknown");
        break;
    }
    label->setText(tr("Account type: %1").arg(accountTypeText));
    label->setVisible(true);

    ignoreSettingsEvent_ = true;

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
            for(int i = dropDown->count() - 1; i >= 0; --i) {
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

    ignoreSettingsEvent_ = false;
    refreshPastePageDefaults();
}

void LogicPasteApp::onUserAvatarUpdated()
{
    qDebug() << "onUserAvatarUpdated()";
    ImageView *imageView = settingsPage_->findChild<ImageView*>("avatarImage");
    AppSettings *appSettings = AppSettings::instance();
    const QByteArray data = appSettings->avatarImage();
    if(!data.isEmpty()) {
        Image displayableImage(data);
        imageView->setImage(displayableImage);
        imageView->setPreferredSize(200.0F, 200.0F);
        imageView->setVisible(true);
    }
    else {
        imageView->setVisible(false);
    }
}

void LogicPasteApp::onUserDetailsError(QString message)
{
    qDebug().nospace() << "onUserDetailsError(" << message << ")";

    bb::system::SystemToast toast;
    if(!message.isEmpty()) {
        toast.setBody(message);
    }
    else {
        toast.setBody(tr("Unable to refresh user details"));
    }
    toast.exec();

    // Refresh from settings to show the old details
    onUserDetailsUpdated();
}

void LogicPasteApp::onCreateAccount() {
    qDebug() << "onCreateAccount()";

    navigator_invoke("http://pastebin.com/signup", 0);
}

void LogicPasteApp::onRequestLogout() {
    qDebug() << "onRequestLogout()";

    pasteModel_->logout();

    emit settingsUpdated();
    refreshMainActions();
}

void LogicPasteApp::onConnectionSettingsChanged()
{
    AppSettings *appSettings = AppSettings::instance();

    CheckBox *sslCheckBox = settingsPage_->findChild<CheckBox *>("sslCheckBox");
    appSettings->setUseSsl(sslCheckBox->isChecked());
}

void LogicPasteApp::onPasteSettingsChanged()
{
    if(ignoreSettingsEvent_) { return; }

    AppSettings *appSettings = AppSettings::instance();

    FormatDropDown *formatDropDown = settingsPage_->findChild<FormatDropDown*>("formatDropDown");
    const QString selectedFormat = formatDropDown->selectedFormat();
    if(!selectedFormat.isNull()) {
        appSettings->setPasteFormatShort(selectedFormat);
    }

    DropDown *expirationDropDown = settingsPage_->findChild<DropDown*>("expirationDropDown");
    appSettings->setPasteExpiration(expirationDropDown->at(expirationDropDown->selectedIndex())->value().toString());

    DropDown *exposureDropDown = settingsPage_->findChild<DropDown*>("exposureDropDown");
    int visibilityValue = exposureDropDown->at(exposureDropDown->selectedIndex())->value().toInt();
    appSettings->setPasteVisibility(static_cast<PasteListing::Visibility>(visibilityValue));

    refreshPastePageDefaults();
}

void LogicPasteApp::onFormatterSettingsChanged()
{
    AppSettings *appSettings = AppSettings::instance();

    CheckBox *formatterEnable = settingsPage_->findChild<CheckBox*>("formatterEnable");
    appSettings->setFormatterEnabled(formatterEnable->isChecked());

    CheckBox *formatterLineNumbering = settingsPage_->findChild<CheckBox*>("formatterLineNumbering");
    appSettings->setFormatterLineNumbering(formatterLineNumbering->isChecked());

    DropDown *formatterStyle = settingsPage_->findChild<DropDown*>("formatterStyle");
    appSettings->setFormatterStyle(formatterStyle->at(formatterStyle->selectedIndex())->value().toString());
}

void LogicPasteApp::refreshMainActions()
{
    ActionItem *refreshAction = historyPage_->findChild<ActionItem*>("refreshAction");
    if(pasteModel_->isAuthenticated()) {
        refreshAction->setEnabled(true);
        tabbedPane_->setProperty("historyPlaceholderText", tr("Tap on the refresh action to load your paste listing"));
        tabbedPane_->setProperty("historyPlaceholderImageSource", "asset:///images/placeholder-refresh.png");
    }
    else {
        refreshAction->setEnabled(false);
        tabbedPane_->setProperty("historyPlaceholderText", tr("You must login to Pastebin before you can load your paste listing"));
        tabbedPane_->setProperty("historyPlaceholderImageSource", "asset:///images/placeholder-settings.png");
    }
}

void LogicPasteApp::refreshPastePageDefaults()
{
    QString pasteContent = pastePage_->findChild<TextArea*>("pasteTextField")->text();
    QString pasteTitle = pastePage_->findChild<TextField*>("pasteTitleField")->text();
    if(!pasteContent.isEmpty() || !pasteTitle.isEmpty()) { return; }

    AppSettings *appSettings = AppSettings::instance();

    FormatDropDown *formatDropDown;
    if(!appSettings->pasteFormatShort().isEmpty()) {
        formatDropDown = pastePage_->findChild<FormatDropDown*>("formatDropDown");
        if(formatDropDown) {
            formatDropDown->selectFormat(appSettings->pasteFormatShort());
        }
    }

    DropDown *dropDown;
    if(!appSettings->pasteExpiration().isEmpty()) {
        dropDown = pastePage_->findChild<DropDown*>("expirationDropDown");
        if(dropDown) {
            for(int i = dropDown->count() - 1; i >= 0; --i) {
                if(dropDown->at(i)->value() == appSettings->pasteExpiration()) {
                    dropDown->setSelectedIndex(i);
                    break;
                }
            }
        }
    }

    int visibilityValue = static_cast<int>(appSettings->pasteVisibility());
    dropDown = pastePage_->findChild<DropDown*>("exposureDropDown");
    if(dropDown) {
        dropDown->setSelectedIndex(visibilityValue);
    }
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

    bb::system::SystemDialog dialog(tr("Okay"));
    dialog.setTitle(tr("Submit Paste"));
    dialog.setBody(tr("Paste successfully submitted"));
    dialog.exec();

    QMetaObject::invokeMethod(pastePage_, "pasteSuccess");
    refreshPastePageDefaults();
}

void LogicPasteApp::onPasteFailed(QString message) {
    qDebug() << "onPasteFailed()";
    disconnect(pasteModel_->pastebin(), SIGNAL(pasteComplete(QString)), this, SLOT(onPasteComplete(QString)));
    disconnect(pasteModel_->pastebin(), SIGNAL(pasteFailed(QString)), this, SLOT(onPasteFailed(QString)));

    bb::system::SystemDialog dialog(tr("Okay"));
    dialog.setTitle(tr("Submit Paste"));
    if(message.isEmpty()) {
        dialog.setBody(tr("Unable to submit paste"));
    }
    else {
        dialog.setBody(message);
    }
    dialog.exec();

    QMetaObject::invokeMethod(pastePage_, "pasteFailed");
}

void LogicPasteApp::onHistoryRefreshComplete(bool success)
{
    QMetaObject::invokeMethod(historyPage_, "onRefreshComplete");
    if(!success) {
        bb::system::SystemToast toast;
        toast.setBody(tr("Unable to refresh your paste listing"));
        toast.exec();
    }
}

void LogicPasteApp::onTrendingRefreshComplete(bool success)
{
    QMetaObject::invokeMethod(trendingPage_, "onRefreshComplete");
    if(!success) {
        bb::system::SystemToast toast;
        toast.setBody(tr("Unable to refresh trending pastes"));
        toast.exec();
    }
}

void LogicPasteApp::onOpenHistoryPaste(QString pasteKey) {
    openPaste(historyNav_, pasteKey);
}

void LogicPasteApp::onOpenTrendingPaste(QString pasteKey) {
    openPaste(trendingNav_, pasteKey);
}

void LogicPasteApp::openPaste(NavigationPane *nav, QString pasteKey) {
    qDebug().nospace() << "onOpenPaste(" << pasteKey << ")";

    ViewPastePage *viewPastePage = new ViewPastePage(pasteModel_, pasteKey);
    connect(viewPastePage, SIGNAL(editPaste(PasteListing,QByteArray)),
        this, SLOT(onEditPaste(PasteListing,QByteArray)),
        Qt::QueuedConnection);
    nav->push(viewPastePage->rootNode());
}

void LogicPasteApp::onDeleteHistoryPaste(QString pasteKey) {
    qDebug().nospace() << "onDeleteHistoryPaste(" << pasteKey << ")";

    const PasteListing listing = pasteModel_->pasteListing(pasteKey);

    bb::system::SystemDialog dialog(tr("Okay"));
    dialog.setTitle(tr("Delete Paste?"));
    if(!listing.title().isEmpty()) {
        dialog.setBody(tr("Are you sure you want to delete \"%1\"?").arg(listing.title()));
    }
    else {
        dialog.setBody(tr("Are you sure you want to delete this paste?"));
    }
    dialog.confirmButton()->setLabel(tr("Delete"));
    dialog.cancelButton()->setLabel(tr("Cancel"));

    if(dialog.exec() == bb::system::SystemUiResult::ConfirmButtonSelection) {
        pasteModel_->deletePaste(pasteKey);
    }
}

void LogicPasteApp::onOpenUrlInBrowser(QString urlString) {
    navigator_invoke(urlString.toLatin1(), 0);
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

void LogicPasteApp::onEditPaste(PasteListing pasteListing, QByteArray rawPaste)
{
    ViewPastePage *viewPastePage = qobject_cast<ViewPastePage*>(sender());
    if(!viewPastePage) { return; }

    NavigationPane *nav = qobject_cast<NavigationPane*>(viewPastePage->rootNode()->parent());
    if(!nav) { return; }

    pastePage_->findChild<TextField*>("pasteTitleField")->setText(
        pasteListing.title());
    pastePage_->findChild<TextArea*>("pasteTextField")->setText(
        QString::fromUtf8(rawPaste.constData(), rawPaste.size()));

    FormatDropDown *dropDown = pastePage_->findChild<FormatDropDown*>("formatDropDown");
    dropDown->selectFormat(pasteListing.formatShort());

    Page *page = nav->pop();
    delete page;
    tabbedPane_->setActiveTab(tabbedPane_->at(0));
}

void LogicPasteApp::onDeletePasteError(PasteListing pasteListing, QString message)
{
    bb::system::SystemToast toast;
    QString displayTitle = pasteListing.title();
    if(displayTitle.isEmpty()) {
        displayTitle = pasteListing.key();
    }
    toast.setBody(tr("Unable to delete paste \"%1\"\n%2").arg(displayTitle).arg(message));
    toast.exec();
}
