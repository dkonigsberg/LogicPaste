#ifndef VIEWPASTEPAGE_H
#define VIEWPASTEPAGE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QByteArray>

#include "PasteListing.h"

namespace bb { namespace cascades {
class Page;
class WebView;
}}

using namespace bb::cascades;

class PasteModel;

class ViewPastePage : public QObject
{
    Q_OBJECT
public:
    ViewPastePage(PasteModel *pasteModel, const QString &pasteKey, QObject *parent=0);
    virtual ~ViewPastePage();

    Page* rootNode() const;

signals:
    void editPaste(PasteListing pasteListing, QByteArray rawPaste);

private slots:
    void onPasteAvailable(PasteListing pasteListing, QByteArray rawPaste);
    void onPasteError(PasteListing pasteListing);
    void onPasteFormatted(const QString& pasteKey, const QString& html);
    void onFormatError();

    // Paste page actions
    void onSavePaste();
    void onSharePaste();
    void onEditPaste();
    void onOpenInBrowser();
    void onCopyPaste();
    void onCopyUrl();

private:
    Page *root_;
    PasteModel *pasteModel_;
    PasteListing pasteListing_;
    QByteArray rawPaste_;
    WebView *webView_;

    void findAndConnectControls();
};

#endif // VIEWPASTEPAGE_H
