#ifndef FORMATDROPDOWN_H
#define FORMATDROPDOWN_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <bb/cascades/CustomControl>

namespace bb { namespace cascades {
class Container;
class Sheet;
class DropDown;
class Option;
}}

using namespace bb::cascades;

class FormatDropDown : public CustomControl
{
    Q_OBJECT
public:
    FormatDropDown(Container *parent=0);
    virtual ~FormatDropDown();

    const QString title() const;
    void setTitle(const QString &title);

    void refreshRecentFormats();

    void selectFormat(const QString& format);
    QString selectedFormat();

signals:
    void titleChanged(QString title);
    void optionAdded(bb::cascades::Option *option);
    void optionRemoved(bb::cascades::Option *option);
    void selectedIndexChanged(int selectedIndex);
    void expandedChanged(bool expanded);

private slots:
    void onMoreSelectedChanged(bool selected);
    void onSheetCancel();
    void onSheetSelectFormat(QString format, QString description);

private:
    DropDown *dropDown_;
    QMap<QString, QString> formatMap_;
    Sheet *formatSheet_;
};

#endif // FORMATDROPDOWN_H
