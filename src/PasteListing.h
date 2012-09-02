#ifndef PASTELISTING_H
#define PASTELISTING_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QMetaType>

class PasteListingData;

class PasteListing {
    Q_ENUMS(Visibility)
public:
    enum Visibility {
        Public = 0,
        Unlisted,
        Private
    };

    PasteListing();
    PasteListing(const PasteListing &other);
    ~PasteListing();
    PasteListing& operator=(const PasteListing& other);

    bool isNull() const;

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

Q_DECLARE_METATYPE(PasteListing)

#endif // PASTELISTING_H
