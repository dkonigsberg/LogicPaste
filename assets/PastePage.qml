import bb.cascades 1.0

Page {
    id: paste
    signal submitPaste()
    titleBar: TitleBar {
        title: qsTr("New Paste")
        visibility: ChromeVisibility.Visible
    }
    content: ScrollView {
        scrollViewProperties {
            scrollMode: ScrollMode.Vertical
        }
        Container {
            leftPadding: 16.0
            rightPadding: 16.0
            topPadding: 16.0
            bottomPadding: 16.0
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            Label {
                text: qsTr("Paste title:")
            }
            TextField {
                id: pasteTitleField
                objectName: "pasteTitleField"
                hintText: qsTr("Paste title (optional)")
            }
            Label {
                text: qsTr("New Paste")
            }
            TextArea {
                id: pasteTextField
                objectName: "pasteTextField"
                preferredHeight: 300
                hintText: qsTr("Text to paste")
                onTextChanging: {
                    if (text.length > 0) {
                        submitAction.enabled = true;
                    } else {
                        submitAction.enabled = false;
                    }
                }
            }
            Divider {
            }
            Label {
                text: qsTr("Optional settings:")
            }
            FormatDropDown {
                id: formatDropDown
                objectName: "formatDropDown"
                enabled: true
            }
            ExpirationDropDown {
                id: expirationDropDown
                objectName: "expirationDropDown"
                enabled: true
            }
            ExposureDropDown {
                id: exposureDropDown
                objectName: "exposureDropDown"
                enabled: true
            }
        }
    }
    actions: [
        ActionItem {
            id: submitAction
            title: qsTr("Submit")
            imageSource: "asset:///images/icon-submit-action.png"
            enabled: false
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                doSubmitPaste();
            }
        }
    ]
    function doSubmitPaste() {
        pasteTitleField.enabled = false;
        pasteTextField.enabled = false;
        formatDropDown.enabled = false;
        expirationDropDown.enabled = false;
        exposureDropDown.enabled = false;
        submitAction.enabled = false;
        paste.submitPaste();
    }
    function pasteSuccess() {
        pasteTitleField.text = "";
        pasteTextField.text = "";
        pasteComplete();
    }
    function pasteFailed() {
        pasteComplete();
    }
    function pasteComplete() {
        pasteTitleField.enabled = true;
        pasteTextField.enabled = true;
        formatDropDown.enabled = true;
        expirationDropDown.enabled = true;
        exposureDropDown.enabled = true;
        submitAction.enabled = true;
    }
}
