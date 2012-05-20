#include <QtCore/QDateTime>

#include "PasteListing.h"

PasteListing::PasteListing(QObject *parent)
    : QObject(parent) {

}

PasteListing::~PasteListing() {

}

QString PasteListing::key() const {
    return key_;
}

QDateTime PasteListing::pasteDate() const {
    return pasteDate_;
}

QString PasteListing::title() const {
    return title_;
}

int PasteListing::size() const {
    return size_;
}

QDateTime PasteListing::expireDate() const {
    return expireDate_;
}

PasteListing::Visibility PasteListing::visibility() const {
    return visibility_;
}

QString PasteListing::formatLong() const {
    return formatLong_;
}

QString PasteListing::formatShort() const {
    return formatShort_;
}

QString PasteListing::url() const {
    return url_;
}

int PasteListing::hits() const {
    return hits_;
}
