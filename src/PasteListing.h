#ifndef PASTELISTING_H
#define PASTELISTING_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>

class Pastebin;

class PasteListing : public QObject {
    Q_OBJECT

    Q_ENUMS(Visibility)
    Q_PROPERTY(QString key READ key FINAL)
    Q_PROPERTY(QDateTime pasteDate READ pasteDate FINAL)
    Q_PROPERTY(QString title READ title FINAL)
    Q_PROPERTY(int size READ size FINAL)
    Q_PROPERTY(QDateTime expireDate READ expireDate FINAL)
    Q_PROPERTY(Visibility visibility READ visibility FINAL)
    Q_PROPERTY(QString formatLong READ formatLong FINAL)
    Q_PROPERTY(QString formatShort READ formatShort FINAL)
    Q_PROPERTY(QString url READ url FINAL)
    Q_PROPERTY(int hits READ hits FINAL)

    friend class Pastebin;

public:
    enum Visibility {
        Public = 0,
        Unlisted,
        Private
    };

    PasteListing(QObject *parent = 0);
    virtual ~PasteListing();

    QString key() const;
    QDateTime pasteDate() const;
    QString title() const;
    int size() const;
    QDateTime expireDate() const;
    Visibility visibility() const;
    QString formatLong() const;
    QString formatShort() const;
    QString url() const;
    int hits() const;

private:
    QString key_;
    QDateTime pasteDate_;
    QString title_;
    int size_;
    QDateTime expireDate_;
    Visibility visibility_;
    QString formatLong_;
    QString formatShort_;
    QString url_;
    int hits_;
};

#endif // PASTELISTING_H
