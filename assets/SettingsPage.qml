import bb.cascades 1.0

Page {
    id: settings
    
    signal requestLogin()
    signal requestLogout()
    signal refreshUserDetails()
    
    content: Container {
        scrollMode: ScrollMode.Vertical
        
        background: Color.create ("#272727")
        layout: StackLayout {
            layoutDirection: LayoutDirection.TopToBottom
            leftPadding: 16
            rightPadding: 16
            topPadding: 16
            bottomPadding: 16
        }
        
        Label {
            text: "Pastebin Account"
            textStyle.color: Color.White
        }
        
        Button {
            id: loginButton
            text: "Log in to Pastebin"
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
            text: "Log out of Pastebin"
            enabled: false
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Fill
            }
            onClicked: {
                settings.requestLogout();
            }
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
        Divider {}
        
        Label {
            text: "Defaults"
            textStyle.color: Color.White
        }
        
        FormatDropDown {
            id: formatDropDown
            enabled: true
            title: "Default format"
        }
        
        ExpirationDropDown {
            id: expirationDropDown
            enabled: true
            title: "Default expiration"
        }
        
        ExposureDropDown {
            id: exposureDropDown
            enabled: true
            title: "Default exposure"
        }
    }
    actions: [
        ActionItem {
            id: refreshAction
            title: "Refresh"
            imageSource: "asset:///images/icon-refresh-action.png"
            enabled: false
            onTriggered: {
                settings.refreshUserDetails();
            }
        }
    ]
    
    onCreationCompleted: {
        cs.settingsUpdated.connect(onSettingsUpdated);
        onSettingsUpdated();
    }
    
    function onSettingsUpdated() {
        var username = cs.getSettingValue("api_user_name");
        var apiKey = cs.getSettingValue("api_user_key");
        
        if(apiKey.length > 0) {
	        loginButton.enabled = false;
	        logoutButton.enabled = true;
	        userLabel.text = "Username: " + username;
	        keyLabel.text = apiKey;
	        userLabel.visible = true;
	        keyLabel.visible = true;
	        refreshAction.enabled = true;
        }
        else {
	        loginButton.enabled = true;
	        logoutButton.enabled = false;
	        userLabel.text = "";
	        keyLabel.text = "";
	        websiteLabel.text = "";
	        emailLabel.text = "";
	        locationLabel.text = "";
	        accountTypeLabel.text = "";
	        userLabel.visible = false;
	        keyLabel.visible = false;
	        websiteLabel.visible = false;
	        emailLabel.visible = false;
	        locationLabel.visible = false;
	        accountTypeLabel.visible = false;
	        refreshAction.enabled = false;
        }
    }
}
