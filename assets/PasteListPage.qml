import bb.cascades 1.0
import bb.system 1.0

NavigationPane {
    id: pasteListNav
    property bool deleteEnabled: false
    property alias placeholderImageSource: placeholderImage.imageSource
    property alias placeholderText: placeholderLabel.text
    Page {
        id: pasteListPage
        objectName: "pasteListPage"
        signal refreshPage()
        content: Container {
            layout: DockLayout {
            }
            ActivityIndicator {
                id: activityIndicator
                preferredWidth: 400
                preferredHeight: 400
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
            Container {
                id: placeholder
                preferredWidth: layoutHandler.layoutFrame.width * 2/3
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                visible: (pasteList.dataModel == null || pasteList.dataModel.childCount([]) == 0) && !activityIndicator.running
                ImageView {
                    id: placeholderImage
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: placeholderLabel
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.textAlign: TextAlign.Center
                }
            }
            ListView {
                id: pasteList
                objectName: "pasteList"
                visible: !placeholder.visible
                property bool deleteEnabled: pasteListNav.deleteEnabled
                signal openPaste(string pasteKey)
                signal copyUrl(string pasteUrl)
                signal deletePaste(string pasteKey)
                listItemComponents: [
                    ListItemComponent {
                        StandardListItem {
                            id: pasteItem
                            title: ListItemData.title
                            description: ListItemData.pasteDate
                            status: ListItemData.formatDescription
                            imageSource: ListItemData.imageSource
                            contextActions: [
                                ActionSet {
                                    title: qsTr("Paste actions")
                                    ActionItem {
                                        title: qsTr("Open")
                                        imageSource: "asset:///images/action-open.png"
                                        enabled: ! ListItemData.isPrivate
                                        onTriggered: {
                                            pasteItem.ListItem.view.openPaste(ListItemData.pasteKey);
                                        }
                                    }
                                    InvokeActionItem {
                                        title: qsTr("Open in browser")
                                        imageSource: "asset:///images/action-open-browser.png"
                                        query {
                                            mimeType: "text/html"
                                            uri: ListItemData.pasteUrl
                                            invokeActionId: "bb.action.OPEN"
                                            invokeTargetId: "sys.browser"
                                        }
                                    }
                                    ActionItem {
                                        title: qsTr("Copy URL")
                                        imageSource: "asset:///images/action-copy-url.png"
                                        onTriggered: {
                                            pasteItem.ListItem.view.copyUrl(ListItemData.pasteUrl);
                                        }
                                    }
                                    InvokeActionItem {
                                        title: qsTr("Share URL")
                                        imageSource: "asset:///images/action-share.png"
                                        query {
                                            mimeType: "text/plain"
                                            data: ListItemData.pasteUrl
                                            invokeActionId: "bb.action.SHARE"
                                        }
                                    }
                                    DeleteActionItem {
                                        title: qsTr("Delete")
                                        imageSource: "asset:///images/action-delete.png"
                                        enabled: pasteItem.ListItem.view.deleteEnabled
                                        onTriggered: {
                                            pasteItem.ListItem.view.deletePaste(ListItemData.pasteKey);
                                        }
                                    }
                                }
                            ]
                        }
                    }
                ]
                onTriggered: {
                    var chosenItem = dataModel.data(indexPath);
                    if (chosenItem.isPrivate) {
                        pasteList.clearSelection();
                    } else {
                        pasteList.openPaste(chosenItem.pasteKey)
                    }
                }
            }
            attachedObjects: [
                LayoutUpdateHandler {
                    id: layoutHandler
                }
            ]
        }
        actions: [
            ActionItem {
                id: refreshAction
                objectName: "refreshAction"
                title: qsTr("Refresh")
                imageSource: "asset:///images/icon-refresh-action.png"
                enabled: false
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    pasteListPage.refreshPage();
                }
            }
        ]
        function onRefreshStarted() {
            refreshAction.enabled = false;
            pasteList.enabled = false;
            activityIndicator.start();
        }
        function onRefreshComplete() {
            activityIndicator.stop();
            pasteList.enabled = true;
            refreshAction.enabled = true;
        }
    }
}
