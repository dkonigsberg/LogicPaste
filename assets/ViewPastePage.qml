import bb.cascades 1.0

Page {
    id: viewPastePage
    property bool pasteLoaded: false
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
            layoutProperties: DockLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
        }
        ScrollView {
            scrollViewProperties {
                scrollMode: ScrollMode.Both
            }
            layoutProperties: DockLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
            }
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
            }
            enabled: false
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            objectName: "shareAction"
            title: qsTr("Share")
            imageSource: "asset:///images/action-share.png"
            onTriggered: {
            }
            enabled: false
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
