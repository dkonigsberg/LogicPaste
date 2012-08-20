import bb.cascades 1.0

DropDown {
    id: expirationDropDown
    objectName: "expirationDropDown"
    title: qsTr("Paste expiration")
    Option {
        selected: true
        text: qsTr("Never")
        value: "N"
    }
    Option {
        text: qsTr("10 Minutes")
        value: "10M"
    }
    Option {
        text: qsTr("1 Hour")
        value: "1H"
    }
    Option {
        text: qsTr("1 Day")
        value: "1D"
    }
    Option {
        text: qsTr("1 Month")
        value: "1M"
    }
}
