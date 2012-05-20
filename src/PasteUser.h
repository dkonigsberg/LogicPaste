#ifndef PASTEUSER_H
#define PASTEUSER_H

#include <QtCore/QSharedDataPointer>

#include "PasteListing.h"

class PasteUserData;

class PasteUser {
public:
    enum Visibility {
        Public = 0,
        Unlisted,
        Private
    };

    enum AccountType {
        Normal = 0,
        Pro = 1
    };

    PasteUser();
    PasteUser(const PasteUser &other);
    ~PasteUser();

    QString username() const;
    void setUsername(const QString& username);
    QString avatarUrl() const;
    void setAvatarUrl(const QString& avatarUrl);
    QString website() const;
    void setWebsite(const QString& website);
    QString email() const;
    void setEmail(const QString& email);
    QString location() const;
    void setLocation(const QString& location);
    AccountType accountType() const;
    void setAccountType(const AccountType accountType);
    QString pasteFormatShort() const;
    void setPasteFormatShort(const QString& pasteFormatShort);
    QString pasteExpiration() const;
    void setPasteExpiration(const QString& pasteExpiration);
    PasteListing::Visibility pasteVisibility() const;
    void setPasteVisibility(const PasteListing::Visibility pasteVisibility);

private:
    QSharedDataPointer<PasteUserData> d;
};

#endif // PASTEUSER_H
