import bb.cascades 1.0

Page {
    id: viewPastePage
    property bool pasteLoaded: false
    signal savePaste()
    signal sharePaste()
    signal editPaste()
    signal openInBrowser()
    signal copyPaste()
    signal copyUrl()
    
    titleBar: TitleBar {
        title: qsTr("Paste")
    }
    content: Container {
        layout: DockLayout {
        }
        ActivityIndicator {
            id: activityIndicator
            objectName: "activityIndicator"
            preferredWidth: 400
            preferredHeight: 400
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
        }
        ScrollView {
            id: scrollView
            scrollViewProperties {
                scrollMode: ScrollMode.Both
                pinchToZoomEnabled: true
            }
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            Container {
                background: Color.LightGray
                WebView {
                    id: webView
                    objectName: "webView"
                    onNavigationRequested: {
                        console.debug("NavigationRequested: " + request.url + " navigationType=" + request.navigationType)
                    }
                    onLoadingChanged: {
                        if (loadRequest.status == WebView.LoadSucceededStatus) {
                            activityIndicator.stop();
                        }
                        if (loadRequest.status == WebView.LoadFailedStatus) {
                            activityIndicator.stop();
                        }
                    }
                    onMinContentScaleChanged: {
                        scrollView.scrollViewProperties.minContentScale = minContentScale;
                    }
                    
                    onMaxContentScaleChanged: {
                        scrollView.scrollViewProperties.maxContentScale = maxContentScale;
                    }
                }
            }
        }
    }
    onCreationCompleted: {
        activityIndicator.start();
    }
    actions: [
        ActionItem {
            objectName: "saveAction"
            title: qsTr("Save")
            imageSource: "asset:///images/action-save.png"
            onTriggered: {
                viewPastePage.savePaste();
            }
            enabled: false
            ActionBar.placement: ActionBarPlacement.OnBar
        },
		InvokeActionItem {
		    objectName: "shareAction";
		    title: qsTr("Share")
		    imageSource: "asset:///images/action-share.png"
		    query {
		        mimeType: "text/plain"
		        invokeActionId: "bb.action.SHARE"
		    }
		    handler : InvokeHandler {
		        onInvoking: {
		            viewPastePage.sharePaste();
		        }
		    }
		    enabled: pasteLoaded
		    ActionBar.placement: ActionBarPlacement.OnBar
		},
        ActionItem {
            objectName: "editAction"
            title: qsTr("Edit Paste")
            imageSource: "asset:///images/action-edit-paste.png"
            onTriggered: {
                viewPastePage.editPaste();
            }
            enabled: pasteLoaded
        },
        ActionItem {
            objectName: "openInBrowserAction"
            title: qsTr("Open in browser")
            imageSource: "asset:///images/action-open-browser.png"
            onTriggered: {
                viewPastePage.openInBrowser();
            }
            enabled: pasteLoaded
        },
        ActionItem {
            objectName: "copyAction"
            title: qsTr("Copy")
            imageSource: "asset:///images/action-copy.png"
            onTriggered: {
                viewPastePage.copyPaste();
            }
            enabled: pasteLoaded
        },
        ActionItem {
            objectName: "copyUrlAction"
            title: qsTr("Copy URL")
            imageSource: "asset:///images/action-copy-url.png"
            onTriggered: {
                viewPastePage.copyUrl();
            }
            enabled: pasteLoaded
        }
    ]
}
