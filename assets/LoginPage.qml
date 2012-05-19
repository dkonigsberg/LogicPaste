import bb.cascades 1.0

Page {
    id: login
    
    signal processLogin(string username, string password)
    signal createAccount()
    
    content: Container {
        background: Color.create("#272727")
        layout: StackLayout {
            layoutDirection: LayoutDirection.TopToBottom
        }
        
        PageTitle {
            titleText: "Log in to Pastebin"
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Fill
            }
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
	            text: "Username"
	            textStyle.color: Color.White
	        }
	        TextField {
	            id: userField
	            clearButtonVisible: false
	            hintText: "Pastebin username"
	            onTextChanging: {
                    updateButtons();
	            }
            }
            
	        Label {
	            text: "Password"
	            textStyle.color: Color.White
	        }
	        TextField {
	            id: passField
	            clearButtonVisible: false
	            hintText: "Pastebin password"
	            onTextChanging: {
	                updateButtons();
	            }
            }
            
            Button {
                id: acceptButton
                text: "Log in"
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
                text: "Create account"
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                onClicked: {
                    login.createAccount();
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
        if(userField.text.length > 0 && passField.text.length > 0) {
            acceptButton.enabled = true;
        }
        else {
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
