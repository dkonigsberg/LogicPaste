import bb.cascades 1.0

Page {
    id: paste
    
    signal submitPaste()
    
    content: Container {
        scrollMode: ScrollMode.Vertical
        
        background: Color.create ("#272727")
        layout: StackLayout {
            layoutDirection: LayoutDirection.TopToBottom
            leftPadding: 16.0
            rightPadding: 16.0
            topPadding: 16.0
            bottomPadding: 16.0
        }
        Label {
            text: "Paste title:"
            textStyle.color: Color.White
        }
        TextField {
            id: pasteTitleField
            objectName: "pasteTitleField"
            hintText: "Paste title (optional)"
        }
        
        Label {
            text: "New Paste"
            textStyle.color: Color.White
        }
        TextArea {
            id: pasteTextField
            objectName: "pasteTextField"
            preferredHeight: 400
            hintText: "Text to paste"
            onTextChanging: {
	            if(text.length > 0) {
	                submitAction.enabled = true;
	            } else {
	                submitAction.enabled = false;
	            }
            }
        }
        Divider {
        }
        Label {
            text: "Optional settings:"
            textStyle.color: Color.White
        }
        FormatDropDown {
            id: formatDropDown
            enabled: true
        }
        ExpirationDropDown {
            id: expirationDropDown
            enabled: true
        }
        ExposureDropDown {
            id: exposureDropDown
            enabled: true
        }
    }
    actions: [
        ActionItem {
            id: submitAction
            title: "Submit"
            imageSource: "asset:///images/icon-submit-action.png"
            enabled: false
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
