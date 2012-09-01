#ifndef PASTEBIN_H
#define PASTEBIN_H

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtNetwork/QNetworkAccessManager>

#include "PasteListing.h"

class QNetworkReply;
class QXmlStreamReader;
class PasteUserData;

class Pastebin : public QObject {
    Q_OBJECT
public:
    explicit Pastebin(QObject *parent=0);
    virtual ~Pastebin();

    void login(const QString& username, const QString& password);

    void submitPaste(const QString& pasteContent, const QString& pasteTitle, const QString& format, const QString& expiration, const PasteListing::Visibility visibility);

    void requestUserDetails();
    void requestUserAvatar();
    void requestHistory();
    void requestTrending();
    void requestFormattedPaste(const QString& pasteUrl, const QString& format);

    void setApiKey(const QString& apiKey);
    QString apiKey() const;

signals:
    void loginComplete(QString apiKey);
    void loginFailed(QString message);
    void pasteComplete(QString pasteUrl);
    void pasteFailed(QString message);
    void userDetailsUpdated();
    void userAvatarUpdated();
    void historyAvailable(QList<PasteListing> *pasteList);
    void trendingAvailable(QList<PasteListing> *pasteList);
    void formattedPasteAvailable(QString pasteUrl, QString html);

private slots:
    void onLoginFinished();
    void onSubmitPasteFinished();
    void onUserDetailsFinished();
    void onUserAvatarFinished();
    void onHistoryFinished();
    void onTrendingFinished();
    void onRequestPasteFinished();
    void onPasteFormatted(const QString& pasteKey, const QString& html);

private:
    void loadRootCert(const QString& fileName);
    void parseUserDetails(QXmlStreamReader& reader, PasteUserData *pasteUser);
    bool processPasteListResponse(QNetworkReply *networkReply, QList<PasteListing> *pasteList);
    bool parsePasteList(QXmlStreamReader& reader, QList<PasteListing> *pasteList);
    void parsePasteElement(QXmlStreamReader& reader, QList<PasteListing> *pasteList);

    QNetworkAccessManager accessManager_;
    QString apiKey_;
};

#endif // PASTEBIN_H
