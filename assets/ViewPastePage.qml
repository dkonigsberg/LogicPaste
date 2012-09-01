import bb.cascades 1.0

Page {
    id: viewPastePage
    property string pasteTitle: qsTr("Paste")
    titleBar: TitleBar {
        title: viewPastePage.pasteTitle
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
            title: qsTr("Save")
            imageSource: "asset:///images/action-save.png"
            onTriggered: {
            }
            enabled: false
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            title: qsTr("Share")
            imageSource: "asset:///images/action-share.png"
            onTriggered: {
            }
            enabled: false
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            title: qsTr("Edit Paste")
            imageSource: "asset:///images/action-edit-paste.png"
            onTriggered: {
            }
            enabled: false
        },
        ActionItem {
            title: qsTr("Open in browser")
            imageSource: "asset:///images/action-open-browser.png"
            onTriggered: {
            }
            enabled: false
        },
        ActionItem {
            title: qsTr("Copy")
            imageSource: "asset:///images/action-copy.png"
            onTriggered: {
            }
            enabled: false
        },
        ActionItem {
            title: qsTr("Copy URL")
            imageSource: "asset:///images/action-copy-url.png"
            onTriggered: {
            }
            enabled: false
        }
    ]
}
