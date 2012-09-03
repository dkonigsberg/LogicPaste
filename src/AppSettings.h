#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QtCore/QtGlobal>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QByteArray>

#include "PasteListing.h"

class AppSettings
{
    Q_ENUMS(AccountType)
public:
    enum AccountType {
        Normal = 0,
        Pro = 1
    };

    static AppSettings* instance();

    void sync();
    void removeUserData();

    QString username() const;
    void setUsername(const QString& username);
    QString apiKey() const;
    void setApiKey(const QString& apiKey);
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

    bool formatterEnabled() const;
    void setFormatterEnabled(bool enabled);
    bool formatterLineNumbering() const;
    void setFormatterLineNumbering(bool lineNumbering);
    QString formatterStyle() const;
    void setFormatterStyle(const QString& style);

    QByteArray avatarImage() const;
    void setAvatarImage(const QByteArray& data);

    QStringList recentFormats() const;
    void setRecentFormats(const QStringList& recentFormats);

private:
    AppSettings();
    Q_DISABLE_COPY(AppSettings);
    static AppSettings *instance_;
    QSettings settings_;
};

#endif // APPSETTINGS_H
