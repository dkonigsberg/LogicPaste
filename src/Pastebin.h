#ifndef PASTEBIN_H
#define PASTEBIN_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QList>
#include "PasteListing.h"

class QXmlStreamReader;

class Pastebin : public QObject {
    Q_OBJECT
public:
    explicit Pastebin(QObject *parent=0);
    virtual ~Pastebin();

    void login(const QString& username, const QString& password);
    void logout();

    void requestTrending();

    QString username() const;
    QString apiKey() const;

signals:
    void loginComplete();
    void loginFailed(QString message);
    void trendingAvailable(QList<PasteListing> *pasteList);

private slots:
    void onLoginFinished();
    void onTrendingFinished();

private:
    void parsePasteElement(QXmlStreamReader& reader, QList<PasteListing> *pasteList);

    QNetworkAccessManager accessManager_;
};

#endif // PASTEBIN_H
