import bb.cascades 1.0

Page {
    id: settings
    signal requestLogin()
    signal requestLogout()
    signal refreshUserDetails()
    signal pasteSettingsChanged()
    signal formatterSettingsChanged()
    content: ScrollView {
        scrollViewProperties {
            scrollMode: ScrollMode.Vertical
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.TopToBottom
                leftPadding: 16
                rightPadding: 16
                topPadding: 16
                bottomPadding: 16
            }
            Label {
                text: qsTr("Pastebin Account:")
                textStyle.base: SystemDefaults.TextStyles.BodyText
            }
            Button {
                id: loginButton
                text: qsTr("Log in to Pastebin")
                enabled: true
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Fill
                }
                onClicked: {
                    settings.requestLogin();
                }
            }
            Button {
                id: logoutButton
                text: qsTr("Log out of Pastebin")
                enabled: false
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Fill
                }
                onClicked: {
                    settings.requestLogout();
                }
            }
            Container {
                layout: StackLayout {
                    layoutDirection: LayoutDirection.LeftToRight
                    leftPadding: 10
                    rightPadding: 10
                }
                ImageView {
                    id: avatarImage
                    objectName: "avatarImage"
                    scalingMethod: ScalingMethod.AspectFit
                    visible: false
                    rightMargin: 10
                    layoutProperties: StackLayoutProperties {
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
                Container {
                    layout: StackLayout {
                        layoutDirection: LayoutDirection.TopToBottom
                    }
                    layoutProperties: StackLayoutProperties {
                        verticalAlignment: VerticalAlignment.Center
                        spaceQuota: 1
                    }
                    Label {
                        id: userLabel
                        objectName: "userLabel"
                        visible: false
                    }
                    Label {
                        id: keyLabel
                        objectName: "keyLabel"
                        visible: false
                        textStyle.base: SystemDefaults.TextStyles.SmallText
                    }
                    Label {
                        id: websiteLabel
                        objectName: "websiteLabel"
                        visible: false
                    }
                    Label {
                        id: emailLabel
                        objectName: "emailLabel"
                        visible: false
                    }
                    Label {
                        id: locationLabel
                        objectName: "locationLabel"
                        visible: false
                    }
                    Label {
                        id: accountTypeLabel
                        objectName: "accountTypeLabel"
                        visible: false
                    }
                }
            }
            ActivityIndicator {
                id: activityIndicator
                preferredWidth: 400
                preferredHeight: 200
                visible: false
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
            }
            Divider {
            }
            Label {
                text: qsTr("Paste defaults:")
                textStyle.base: SystemDefaults.TextStyles.BodyText
            }
            FormatDropDown {
                objectName: "formatDropDown"
                enabled: true
                title: qsTr("Default format")
                onSelectedIndexChanged: {
                    settings.pasteSettingsChanged()
                }
            }
            ExpirationDropDown {
                enabled: true
                title: qsTr("Default expiration")
                onSelectedIndexChanged: {
                    settings.pasteSettingsChanged()
                }
            }
            ExposureDropDown {
                enabled: true
                title: qsTr("Default exposure")
                onSelectedIndexChanged: {
                    settings.pasteSettingsChanged()
                }
            }
            Divider {
            }
            Label {
                text: qsTr("Formatter settings:")
                textStyle.base: SystemDefaults.TextStyles.BodyText
            }
            CheckBox {
                id: formatterEnable
                objectName: "formatterEnable"
                text: qsTr("Enable paste formatting")
                checked: true
                onCheckedChanged: {
                    settings.formatterSettingsChanged()
                }
            }
            CheckBox {
                id: formatterLineNumbering
                objectName: "formatterLineNumbering"
                text: qsTr("Line numbering")
                checked: true
                onCheckedChanged: {
                    settings.formatterSettingsChanged()
                }
                enabled: formatterEnable.checked
            }
            StyleDropDown {
                id: formatterStyle
                objectName: "formatterStyle"
                title: qsTr("Style")
                onSelectedIndexChanged: {
                    settings.formatterSettingsChanged()
                }
                enabled: formatterEnable.checked
            }
        }
    }
    actions: [
        ActionItem {
            id: refreshAction
            title: qsTr("Refresh")
            imageSource: "asset:///images/icon-refresh-action.png"
            enabled: false
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                startRefreshUserDetails();
            }
        }
    ]
    function onSettingsUpdated() {
        if (model.isAuthenticated()) {
            loginButton.enabled = false;
            logoutButton.enabled = true;
            refreshAction.enabled = true;
        } else {
            loginButton.enabled = true;
            logoutButton.enabled = false;
            userLabel.text = "";
            keyLabel.text = "";
            websiteLabel.text = "";
            emailLabel.text = "";
            locationLabel.text = "";
            accountTypeLabel.text = "";
            avatarImage.image = null;
            userLabel.visible = false;
            keyLabel.visible = false;
            websiteLabel.visible = false;
            emailLabel.visible = false;
            locationLabel.visible = false;
            accountTypeLabel.visible = false;
            avatarImage.visible = false;
            refreshAction.enabled = false;
        }
    }
    function startRefreshUserDetails() {
        websiteLabel.text = "";
        emailLabel.text = "";
        locationLabel.text = "";
        accountTypeLabel.text = "";
        websiteLabel.visible = false;
        emailLabel.visible = false;
        locationLabel.visible = false;
        accountTypeLabel.visible = false;
        refreshAction.enabled = false;
        activityIndicator.visible = true;
        activityIndicator.start();
        settings.refreshUserDetails();
    }
    function userDetailsRefreshed() {
        activityIndicator.stop();
        activityIndicator.visible = false;
        refreshAction.enabled = true;
    }
}
