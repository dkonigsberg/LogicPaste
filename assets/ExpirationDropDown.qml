import bb.cascades 1.0

DropDown {
    id: expirationDropDown
    objectName: "expirationDropDown"
    title: "Paste expiration"
    Option {
        selected: true
        text: "Never"
        value: "N"
    }
    Option {
        text: "10 Minutes"
        value: "10M"
    }
    Option {
        text: "1 Hour"
        value: "1H"
    }
    Option {
        text: "1 Day"
        value: "1D"
    }
    Option {
        text: "1 Month"
        value: "1M"
    }
}
