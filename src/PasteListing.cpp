#include <QtCore/QSharedData>
#include <QtCore/QDateTime>

#include "PasteListing.h"

class PasteListingData: public QSharedData {
public:
    PasteListingData() : pasteSize(0), visibility(PasteListing::Public), hits(0) {}
    QString key;
    QDateTime pasteDate;
    QString title;
    int pasteSize;
    QDateTime expireDate;
    PasteListing::Visibility visibility;
    QString formatLong;
    QString formatShort;
    QString url;
    int hits;
};

PasteListing::PasteListing() {
    d = new PasteListingData();
}

PasteListing::~PasteListing() {
}

PasteListing::PasteListing(const PasteListing &other)
    : d(other.d) {
}

PasteListing& PasteListing::operator=(const PasteListing& other)
{
    d = other.d;
    return *this;
}

QString PasteListing::key() const {
    return d->key;
}
void PasteListing::setKey(const QString& key) {
    d->key = key;
}

QDateTime PasteListing::pasteDate() const {
    return d->pasteDate;
}
void PasteListing::setPasteDate(const QDateTime& pasteDate) {
    d->pasteDate = pasteDate;
}

QString PasteListing::title() const {
    return d->title;
}
void PasteListing::setTitle(const QString& title) {
    d->title = title;
}

int PasteListing::pasteSize() const {
    return d->pasteSize;
}
void PasteListing::setPasteSize(const int pasteSize) {
    d->pasteSize = pasteSize;
}

QDateTime PasteListing::expireDate() const {
    return d->expireDate;
}
void PasteListing::setExpireDate(const QDateTime& expireDate) {
    d->expireDate = expireDate;
}

PasteListing::Visibility PasteListing::visibility() const {
    return d->visibility;
}
void PasteListing::setVisibility(const Visibility visibility) {
    d->visibility = visibility;
}

QString PasteListing::formatLong() const {
    return d->formatLong;
}
void PasteListing::setFormatLong(const QString& formatLong) {
    d->formatLong = formatLong;
}

QString PasteListing::formatShort() const {
    return d->formatShort;
}
void PasteListing::setFormatShort(const QString& formatShort) {
    d->formatShort = formatShort;
}

QString PasteListing::url() const {
    return d->url;
}
void PasteListing::setUrl(const QString& url) {
    d->url = url;
}

int PasteListing::hits() const {
    return d->hits;
}
void PasteListing::setHits(const int hits) {
    d->hits = hits;
}
