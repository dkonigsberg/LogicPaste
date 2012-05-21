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
        ActivityIndicator {
            id: activityIndicator
            preferredWidth: 400
            preferredHeight: 200
            visible: false
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
            }
        }
        
        Divider {}
        
        Label {
            text: "Defaults"
            textStyle.color: Color.White
        }
        
        FormatDropDown {
            enabled: true
            title: "Default format"
        }
        
        ExpirationDropDown {
            enabled: true
            title: "Default expiration"
        }
        
        ExposureDropDown {
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
                startRefreshUserDetails();
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
	        keyLabel.text = "API user key: " + apiKey;
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
