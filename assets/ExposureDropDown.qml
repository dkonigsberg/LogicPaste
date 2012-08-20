import bb.cascades 1.0

DropDown {
    id: exposureDropDown
    objectName: "exposureDropDown"
    title: qsTr("Paste exposure")
    Option {
        selected: true
        text: qsTr("Public")
        value: "0"
    }
    Option {
        text: qsTr("Unlisted")
        value: "1"
    }
    Option {
        text: qsTr("Private")
        value: "2"
    }
}
