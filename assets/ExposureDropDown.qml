import bb.cascades 1.0

DropDown {
    title: "Paste exposure"
    Option {
        selected: true
        text: "Public"
        value: "0"
    }
    Option {
        text: "Unlisted"
        value: "1"
    }
    Option {
        text: "Private"
        value: "2"
    }
}
