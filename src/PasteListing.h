#ifndef PASTELISTING_H
#define PASTELISTING_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QSharedDataPointer>

class PasteListingData;

class PasteListing {

public:
    enum Visibility {
        Public = 0,
        Unlisted,
        Private
    };

    PasteListing();
    PasteListing(const PasteListing &other);
    ~PasteListing();

    QString key() const;
    void setKey(const QString& key);
    QDateTime pasteDate() const;
    void setPasteDate(const QDateTime& pasteDate);
    QString title() const;
    void setTitle(const QString& title);
    int pasteSize() const;
    void setPasteSize(const int pasteSize);
    QDateTime expireDate() const;
    void setExpireDate(const QDateTime& expireDate);
    Visibility visibility() const;
    void setVisibility(const Visibility visibility);
    QString formatLong() const;
    void setFormatLong(const QString& formatLong);
    QString formatShort() const;
    void setFormatShort(const QString& formatShort);
    QString url() const;
    void setUrl(const QString& url);
    int hits() const;
    void setHits(const int hits);

private:
    QSharedDataPointer<PasteListingData> d;
};

#endif // PASTELISTING_H
