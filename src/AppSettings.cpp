#include "AppSettings.h"

#include <QtCore/QMutex>

AppSettings *AppSettings::instance_ = NULL;

AppSettings* AppSettings::instance()
{
    static QMutex mutex;
    if(!instance_) {
        mutex.lock();
        if(!instance_) {
            instance_ = new AppSettings();
        }
        mutex.unlock();
    }
    return instance_;
}

AppSettings::AppSettings()
{
}

void AppSettings::sync()
{
    settings_.sync();
}

void AppSettings::removeUserData()
{
    settings_.remove("api_user_name");
    settings_.remove("api_user_key");
    settings_.remove("user_avatar_url");
    settings_.remove("user_website");
    settings_.remove("user_email");
    settings_.remove("user_location");
    settings_.remove("user_account_type");
    settings_.remove("user_format_short");
    settings_.remove("user_expiration");
    settings_.remove("user_private");
}

QString AppSettings::username() const {
    return settings_.value("api_user_name", "").toString();
}
void AppSettings::setUsername(const QString& username) {
    settings_.setValue("api_user_name", username);
}

QString AppSettings::apiKey() const {
    return settings_.value("api_user_key", "").toString();
}
void AppSettings::setApiKey(const QString& apiKey) {
    settings_.setValue("api_user_key", apiKey);
}

QString AppSettings::avatarUrl() const {
    return settings_.value("user_avatar_url", "").toString();
}
void AppSettings::setAvatarUrl(const QString& avatarUrl) {
    settings_.setValue("user_avatar_url", avatarUrl);
}

QString AppSettings::website() const {
    return settings_.value("user_website", "").toString();
}
void AppSettings::setWebsite(const QString& website) {
    settings_.setValue("user_website", website);
}

QString AppSettings::email() const {
    return settings_.value("user_email", "").toString();
}
void AppSettings::setEmail(const QString& email) {
    settings_.setValue("user_email", email);
}

QString AppSettings::location() const {
    return settings_.value("user_location", "").toString();
}
void AppSettings::setLocation(const QString& location) {
    settings_.setValue("user_location", location);
}

AppSettings::AccountType AppSettings::accountType() const {
    return static_cast<AppSettings::AccountType>(settings_.value("user_account_type", 0).toInt());
}
void AppSettings::setAccountType(const AppSettings::AccountType accountType) {
    settings_.setValue("user_account_type", static_cast<int>(accountType));
}

QString AppSettings::pasteFormatShort() const {
    return settings_.value("user_format_short", "").toString();
}
void AppSettings::setPasteFormatShort(const QString& pasteFormatShort) {
    settings_.setValue("user_format_short", pasteFormatShort);
}

QString AppSettings::pasteExpiration() const {
    return settings_.value("user_expiration", "").toString();
}
void AppSettings::setPasteExpiration(const QString& pasteExpiration) {
    settings_.setValue("user_expiration", pasteExpiration);
}

PasteListing::Visibility AppSettings::pasteVisibility() const {
    return static_cast<PasteListing::Visibility>(settings_.value("user_private", 0).toInt());
}
void AppSettings::setPasteVisibility(const PasteListing::Visibility pasteVisibility) {
    settings_.setValue("user_private", static_cast<int>(pasteVisibility));
}
