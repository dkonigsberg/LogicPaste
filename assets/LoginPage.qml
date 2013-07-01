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
            orientation: LayoutOrientation.TopToBottom
        }
        Divider {
        }
        Container {
            leftPadding: 16
            rightPadding: 16
            bottomPadding: 16
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            Label {
                text: qsTr("Username")
            }
            TextField {
                id: userField
                clearButtonVisible: false
                hintText: qsTr("Pastebin username")
                inputMode: TextFieldInputMode.Text
                input.flags: TextInputFlag.AutoCapitalizationOff
	                | TextInputFlag.AutoCorrectionOff
	                | TextInputFlag.AutoPeriodOff
	                | TextInputFlag.WordSubstitutionOff
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
            Container {
                topMargin: 20
                bottomMargin: 20
                horizontalAlignment: HorizontalAlignment.Center
                Button {
                    id: acceptButton
                    visible: !activityIndicator.running
                    topMargin: 10
                    bottomMargin: 10
                    text: qsTr("Log in")
                    enabled: false
                    horizontalAlignment: HorizontalAlignment.Fill
                    onClicked: {
                        startLoginProcess();
                    }
                }
                Button {
                    id: createAccountButton
                    visible: !activityIndicator.running
                    topMargin: 10
                    bottomMargin: 10
                    text: qsTr("Create account")
                    horizontalAlignment: HorizontalAlignment.Fill
                    onClicked: {
                        login.createAccount();
                    }
                }
                Button {
                    id: cancelButton
                    topMargin: 10
                    bottomMargin: 10
                    text: qsTr("Cancel")
                    horizontalAlignment: HorizontalAlignment.Fill
                    onClicked: {
                        login.cancel();
                    }
                }
            }
        }
        Container {
            layout: DockLayout {
            }
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            horizontalAlignment: HorizontalAlignment.Center
            ActivityIndicator {
                id: activityIndicator
                preferredWidth: 200
                preferredHeight: 200
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top
            }
            Label {
                id: errorText
                visible: false
                multiline: true
                textStyle.color: Color.Red
                textStyle.textAlign: TextAlign.Center
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
