import bb.cascades 1.0

Page {
    id: login
    signal processLogin(string username, string password)
    signal createAccount()
    signal cancel()
    titleBar: TitleBar {
        title: qsTr("Log in to Pastebin")
    }
    content: Container {
        layout: StackLayout {
            layoutDirection: LayoutDirection.TopToBottom
        }
        Divider {
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.TopToBottom
                leftPadding: 16
                rightPadding: 16
                bottomPadding: 16
            }
            Label {
                text: qsTr("Username")
            }
            TextField {
                id: userField
                clearButtonVisible: false
                hintText: qsTr("Pastebin username")
                inputMode: TextFieldInputMode.Text
                onTextChanging: {
                    updateButtons();
                }
            }
            Label {
                text: qsTr("Password")
            }
            TextField {
                id: passField
                clearButtonVisible: false
                hintText: qsTr("Pastebin password")
                inputMode: TextFieldInputMode.Password
                onTextChanging: {
                    updateButtons();
                }
            }
            Button {
                id: acceptButton
                text: qsTr("Log in")
                enabled: false
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                onClicked: {
                    startLoginProcess();
                }
            }
            Button {
                id: createAccountButton
                text: qsTr("Create account")
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                onClicked: {
                    login.createAccount();
                }
            }
            Button {
                id: cancelButton
                text: qsTr("Cancel")
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                onClicked: {
                    login.cancel();
                }
            }
        }
        Container {
            layout: DockLayout {
            }
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
            }
            ActivityIndicator {
                id: activityIndicator
                preferredWidth: 400
                preferredHeight: 400
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Top
                }
            }
            TextArea {
                id: errorText
                visible: false
                backgroundVisible: false
                textStyle.color: Color.Red
                textStyle.alignment: TextAlignment.Center
            }
        }
    }
    function updateButtons() {
        if (userField.text.length > 0 && passField.text.length > 0) {
            acceptButton.enabled = true;
        } else {
            acceptButton.enabled = false;
        }
    }
    function startLoginProcess() {
        userField.enabled = false;
        passField.enabled = false;
        acceptButton.enabled = false;
        createAccountButton.enabled = false;
        errorText.text = "";
        errorText.visible = false;
        activityIndicator.start();
        cs.loginFailed.connect(onLoginFailed);
        login.processLogin(userField.text, passField.text);
    }
    function onLoginFailed(message) {
        cs.loginFailed.disconnect(onLoginFailed);
        userField.enabled = true;
        passField.enabled = true;
        acceptButton.enabled = true;
        createAccountButton.enabled = true;
        activityIndicator.stop();
        errorText.text = message;
        errorText.visible = true;
    }
}
