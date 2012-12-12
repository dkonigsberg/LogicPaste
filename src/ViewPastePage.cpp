#include "ViewPastePage.h"

#include <QtCore/QFile>
#include <QtGui/QTextDocument>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>
#include <bb/cascades/TitleBar>
#include <bb/cascades/WebView>
#include <bb/cascades/WebSettings>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/InvokeHandler>
#include <bb/cascades/pickers/FilePicker>
#include <bb/system/Clipboard>
#include <bb/system/InvokeQueryTargetsRequest>
#include <bb/system/InvokeQueryTargetsReply>
#include <bb/system/InvokeTarget>
#include <bb/system/InvokeAction>
#include <bb/system/InvokeReply>
#include <bb/system/SystemToast>

#include <bps/navigator.h>

#include "PasteModel.h"
#include "Pastebin.h"
#include "PasteFormatter.h"

ViewPastePage::ViewPastePage(PasteModel *pasteModel, const QString &pasteKey, QObject *parent)
    : QObject(parent), pasteModel_(pasteModel)
{
    QmlDocument *qml = QmlDocument::create("asset:///ViewPastePage.qml").parent(this);
    qml->setContextProperty("cs", this);
    root_ = qml->createRootObject<Page>();
    connect(root_, SIGNAL(destroyed()), this, SLOT(deleteLater()));

    findAndConnectControls();

    pasteListing_ = pasteModel_->pasteListing(pasteKey);
    root_->setProperty("pasteUrl", pasteListing_.url());
    if(!pasteListing_.title().isEmpty()) {
        root_->titleBar()->setTitle(pasteListing_.title());
    }

    connect(pasteModel_, SIGNAL(pasteAvailable(PasteListing,QByteArray)), this, SLOT(onPasteAvailable(PasteListing,QByteArray)));
    connect(pasteModel_, SIGNAL(pasteError(PasteListing)), this, SLOT(onPasteError(PasteListing)));

    pasteModel_->requestPaste(pasteKey);
}

ViewPastePage::~ViewPastePage()
{
}

void ViewPastePage::findAndConnectControls()
{
    connect(root_, SIGNAL(savePaste()), this, SLOT(onSavePaste()));
    connect(root_, SIGNAL(sharePaste()), this, SLOT(onSharePaste()));
    connect(root_, SIGNAL(shareUrl()), this, SLOT(onShareUrl()));
    connect(root_, SIGNAL(editPaste()), this, SLOT(onEditPaste()));
    connect(root_, SIGNAL(openInBrowser()), this, SLOT(onOpenInBrowser()));
    connect(root_, SIGNAL(copyPaste()), this, SLOT(onCopyPaste()));
    connect(root_, SIGNAL(copyUrl()), this, SLOT(onCopyUrl()));

    webView_ = root_->findChild<WebView*>("webView");
}

Page* ViewPastePage::rootNode() const
{
    return root_;
}

void ViewPastePage::onPasteAvailable(PasteListing pasteListing, QByteArray rawPaste)
{
    if(pasteListing.key() != pasteListing_.key()) { return; }
    disconnect(pasteModel_, SIGNAL(pasteAvailable(PasteListing,QByteArray)), this, SLOT(onPasteAvailable(PasteListing,QByteArray)));
    disconnect(pasteModel_, SIGNAL(pasteError(PasteListing)), this, SLOT(onPasteError(PasteListing)));
    pasteListing_ = pasteListing;
    rawPaste_ = rawPaste;

    PasteFormatter *formatter = new PasteFormatter(this);
    connect(formatter, SIGNAL(pasteFormatted(QString,QString)),
        this, SLOT(onPasteFormatted(QString,QString)));
    connect(formatter, SIGNAL(formatError()), this, SLOT(onFormatError()));
    formatter->formatPaste(pasteListing.key(), pasteModel_->lexerForFormat(pasteListing.formatShort()), rawPaste);
}

void ViewPastePage::onPasteError(PasteListing pasteListing)
{
    if(pasteListing.key() != pasteListing_.key()) { return; }
    disconnect(pasteModel_, SIGNAL(pasteAvailable(PasteListing,QByteArray)), this, SLOT(onPasteAvailable(PasteListing,QByteArray)));
    disconnect(pasteModel_, SIGNAL(pasteError(PasteListing)), this, SLOT(onPasteError(PasteListing)));
    pasteListing_ = pasteListing;

    QString errorHtml = QString("<html><body>%1</body></html>").arg(tr("Error retrieving paste"));
    webView_->setHtml(errorHtml);
}

void ViewPastePage::onPasteFormatted(const QString& pasteKey, const QString& html)
{
    PasteFormatter *formatter = qobject_cast<PasteFormatter*>(sender());
    Q_UNUSED(pasteKey);
    disconnect(formatter, SIGNAL(pasteFormatted(QString,QString)),
        this, SLOT(onPasteFormatted(QString,QString)));
    disconnect(formatter, SIGNAL(formatError()), this, SLOT(onFormatError()));

    webView_->setHtml(html);
    root_->setProperty("pasteLoaded", true);
}

void ViewPastePage::onFormatError()
{
    PasteFormatter *formatter = qobject_cast<PasteFormatter*>(sender());
    disconnect(formatter, SIGNAL(pasteFormatted(QString,QString)),
        this, SLOT(onPasteFormatted(QString,QString)));
    disconnect(formatter, SIGNAL(formatError()), this, SLOT(onFormatError()));

    qWarning() << "Error formatting paste, falling back to raw output";

    const QString rawPasteString = QString::fromUtf8(rawPaste_.constData(), rawPaste_.size());

    QTextDocument document(rawPasteString);
    const QString pasteHtml = document.toHtml();
    webView_->settings()->setMinimumFontSize(36);
    webView_->setHtml(pasteHtml);
}

void ViewPastePage::onSavePaste()
{
    pickers::FilePicker* filePicker = new pickers::FilePicker();
    filePicker->setType(pickers::FileType::Document);
    filePicker->setMode(pickers::FilePickerMode::Saver);

    QString baseName = pasteListing_.title().trimmed();
    if(baseName.isEmpty()) {
        baseName = pasteListing_.key();
    }
    const QString saveName = baseName.append(".txt");
    filePicker->setDefaultSaveFileNames(QStringList() << saveName);

    connect(filePicker, SIGNAL(fileSelected(QStringList)), this, SLOT(onPickerFileSelected(QStringList)), Qt::QueuedConnection);
    connect(filePicker, SIGNAL(canceled()), this, SLOT(onPickerCanceled()));
    filePicker->open();
}

void ViewPastePage::onPickerFileSelected(const QStringList& selectedFiles)
{
    pickers::FilePicker *picker = qobject_cast<pickers::FilePicker*>(sender());
    picker->deleteLater();
    if(selectedFiles.length() < 1 || selectedFiles[0].isEmpty()) { return; }
    const QString filename = selectedFiles[0];

    qDebug() << "Saving paste" << pasteListing_.key() << "to file:" << filename;
    QFile pasteFile(filename);

    bool success = true;
    if(pasteFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if(pasteFile.write(rawPaste_) < 0) {
            qWarning() << "Unable to write paste to file:" << pasteFile.errorString();
            success = false;
        }
    }
    else {
        qWarning() << "Unable to write paste to file:" << pasteFile.errorString();
        success = false;
    }
    pasteFile.close();

    if(success) {
        qDebug() << "Paste saved successfully";
        bb::system::SystemToast toast;
        toast.setBody(tr("Paste saved to file %1").arg(pasteFile.fileName()));
        toast.exec();
    }
    else {
        bb::system::SystemToast toast;
        toast.setBody(tr("'%1' error saving paste to file %2").arg(pasteFile.errorString()).arg(pasteFile.fileName()));
        toast.exec();
    }
}

void ViewPastePage::onPickerCanceled()
{
    pickers::FilePicker *picker = qobject_cast<pickers::FilePicker*>(sender());
    picker->deleteLater();
}

void ViewPastePage::onSharePaste()
{
    InvokeActionItem *invokeAction = root_->findChild<InvokeActionItem*>("shareAction");
    if(!invokeAction) {
        qWarning() << "Could not find share action";
        return;
    }

    invokeAction->setInvocationData(rawPaste_);
    invokeAction->handler()->confirm();
}

void ViewPastePage::onShareUrl()
{
    InvokeActionItem *invokeAction = root_->findChild<InvokeActionItem*>("shareUrlAction");
    if(!invokeAction) {
        qWarning() << "Could not find share URL action";
        return;
    }

    invokeAction->setInvocationData(pasteListing_.url().toUtf8());
    invokeAction->handler()->confirm();
}

void ViewPastePage::onEditPaste() {
    emit editPaste(pasteListing_, rawPaste_);
}

void ViewPastePage::onOpenInBrowser()
{
    navigator_invoke(pasteListing_.url().toUtf8(), 0);
}

void ViewPastePage::onCopyPaste()
{
    if(rawPaste_.isEmpty()) { return; }
    bb::system::Clipboard clipboard;
    if(clipboard.clear()) {
        clipboard.insert("text/plain", rawPaste_);
    }
}

void ViewPastePage::onCopyUrl()
{
    bb::system::Clipboard clipboard;
    if(clipboard.clear()) {
        clipboard.insert("text/plain", pasteListing_.url().toUtf8());
    }
}
