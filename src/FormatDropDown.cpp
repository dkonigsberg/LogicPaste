#include "FormatDropDown.h"

#include <QtCore/QUrl>
#include <bb/cascades/DropDown>
#include <bb/cascades/Option>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>
#include <bb/cascades/Sheet>
#include <bb/cascades/XmlDataModel>

#include "AppSettings.h"

FormatDropDown::FormatDropDown(Container *parent)
    : CustomControl(parent), formatSheet_(NULL)
{
    dropDown_ = new DropDown();
    dropDown_->setTitle("Paste format");

    Option *textOption = new Option();
    textOption->setText("None");
    textOption->setValue("text");
    textOption->setSelected(true);
    dropDown_->add(textOption);

    Option *moreOption = new Option();
    moreOption->setText("Show more choices...");
    connect(moreOption, SIGNAL(selectedChanged(bool)), this, SLOT(onMoreSelectedChanged(bool)));
    dropDown_->add(moreOption);

    XmlDataModel xmlModel;
    xmlModel.setSource(QUrl("models/paste_formats.xml"));
    int items = xmlModel.childCount(QVariantList());
    QVariantList index;
    index.append(0);
    for(int i = 0; i < items; i++) {
        index[0] = i;
        QVariantMap map = xmlModel.data(index).toMap();
        formatMap_.insert(map["name"].toString(), map["description"].toString());
    }

    refreshRecentFormats();

    setRoot(dropDown_);
}

FormatDropDown::~FormatDropDown()
{
}

const QString FormatDropDown::title() const
{
    return dropDown_->title();
}

void FormatDropDown::setTitle(const QString& title)
{
    dropDown_->setTitle(title);
}

void FormatDropDown::refreshRecentFormats()
{
    const QString selected = selectedFormat();

    for(int i = dropDown_->optionCount() - 2; i > 0; --i) {
        Option *option = dropDown_->at(i);
        if(dropDown_->remove(option)) {
            option->deleteLater();
        }
    }

    QStringList recentList = AppSettings::instance()->recentFormats();
    foreach(const QString format, recentList) {
        Option *option = new Option();
        option->setText(formatMap_.value(format));
        option->setValue(format);
        dropDown_->insert(dropDown_->optionCount() - 1, option);
    }

    selectFormat(selected);
}

void FormatDropDown::selectFormat(const QString& format)
{
    bool selected = false;
    for(int i = dropDown_->optionCount() - 1; i >= 0; --i) {
        if(dropDown_->at(i)->value() == format) {
            dropDown_->setSelectedIndex(i);
            selected = true;
            break;
        }
    }

    if(!selected && formatMap_.contains(format)) {
        Option *option = new Option();
        option->setText(formatMap_.value(format));
        option->setValue(format);
        dropDown_->insert(1, option);
        dropDown_->setSelectedIndex(1);
        int count = dropDown_->optionCount();
        if(count > 6) {
            dropDown_->remove(dropDown_->at(5));
        }
        QStringList recentList;
        for(int i = 1; i < count - 1; i++) {
            recentList.append(dropDown_->at(i)->value().toString());
        }
        AppSettings::instance()->setRecentFormats(recentList);
    }
}

QString FormatDropDown::selectedFormat()
{
    return dropDown_->at(dropDown_->selectedIndex())->value().toString();
}

void FormatDropDown::onMoreSelectedChanged(bool selected)
{
    if(selected) {
        if(!formatSheet_) {
            QmlDocument *qml = QmlDocument::create(this, "FormatsListPage.qml");
            if(qml->hasErrors()) { return; }
            Page *page = qml->createRootNode<Page>();
            if(!page) { return; }

            connect(page, SIGNAL(cancel()), this, SLOT(onSheetCancel()));
            connect(page, SIGNAL(selectFormat(QString, QString)), this, SLOT(onSheetSelectFormat(QString, QString)));
            formatSheet_ = Sheet::create().content(page);
        }
        formatSheet_->setVisible(true);
    }
}

void FormatDropDown::onSheetCancel()
{
    formatSheet_->setVisible(false);
}

void FormatDropDown::onSheetSelectFormat(QString format, QString description)
{
    formatSheet_->setVisible(false);
    selectFormat(format);
}
