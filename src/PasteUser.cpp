#include <QtCore/QString>
#include <QtCore/QSharedData>

#include "PasteListing.h"
#include "PasteUser.h"

class PasteUserData: public QSharedData {
public:
    PasteUserData() {}
    QString username;
    QString avatarUrl;
    QString website;
    QString email;
    QString location;
    PasteUser::AccountType accountType;
    QString pasteFormatShort;
    QString pasteExpiration;
    PasteListing::Visibility pasteVisibility;
};

PasteUser::PasteUser() {
    d = new PasteUserData();
}

PasteUser::~PasteUser() {
}

PasteUser::PasteUser(const PasteUser &other)
    : d(other.d) {
}

QString PasteUser::username() const {
    return d->username;
}
void PasteUser::setUsername(const QString& username) {
    d->username = username;
}

QString PasteUser::avatarUrl() const {
    return d->avatarUrl;
}
void PasteUser::setAvatarUrl(const QString& avatarUrl) {
    d->avatarUrl = avatarUrl;
}

QString PasteUser::website() const {
    return d->website;
}
void PasteUser::setWebsite(const QString& website) {
    d->website = website;
}

QString PasteUser::email() const {
    return d->email;
}
void PasteUser::setEmail(const QString& email) {
    d->email = email;
}

QString PasteUser::location() const {
    return d->location;
}
void PasteUser::setLocation(const QString& location) {
    d->location = location;
}

PasteUser::AccountType PasteUser::accountType() const {
    return d->accountType;
}
void PasteUser::setAccountType(const PasteUser::AccountType accountType) {
    d->accountType = accountType;
}

QString PasteUser::pasteFormatShort() const {
    return d->pasteFormatShort;
}
void PasteUser::setPasteFormatShort(const QString& pasteFormatShort) {
    d->pasteFormatShort = pasteFormatShort;
}

QString PasteUser::pasteExpiration() const {
    return d->pasteExpiration;
}
void PasteUser::setPasteExpiration(const QString& pasteExpiration) {
    d->pasteExpiration = pasteExpiration;
}

PasteListing::Visibility PasteUser::pasteVisibility() const {
    return d->pasteVisibility;
}
void PasteUser::setPasteVisibility(const PasteListing::Visibility pasteVisibility) {
    d->pasteVisibility = pasteVisibility;
}
