#ifndef PASTEBIN_H
#define PASTEBIN_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>

class Pastebin : public QObject {
    Q_OBJECT
public:
    explicit Pastebin(QObject *parent=0);
    virtual ~Pastebin();

    void login(const QString& username, const QString& password);
    void logout();

    QString username() const;
    QString apiKey() const;

signals:
    void loginComplete();
    void loginFailed(QString message);

private slots:
    void onLoginFinished();

private:
    QNetworkAccessManager accessManager_;
};

#endif // PASTEBIN_H
