import bb.cascades 1.0

Page {
    id: settings
    
    signal requestLogin()
    signal requestLogout()
    
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
            visible: false
        }
        Label {
            id: keyLabel
            visible: false
            textStyle.base: SystemDefaults.TextStyles.SmallText
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
        }
        else {
	        loginButton.enabled = true;
	        logoutButton.enabled = false;
	        userLabel.text = "";
	        keyLabel.text = "";
	        userLabel.visible = false;
	        keyLabel.visible = false;
        }
    }
}
