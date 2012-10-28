import bb.cascades 1.0

NavigationPane {
    id: pasteListNav
    Page {
        id: pasteListPage
        objectName: "pasteListPage"
        signal refreshPage()
        content: Container {
            Container {
                layout: DockLayout {
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                ActivityIndicator {
                    id: activityIndicator
                    preferredWidth: 400
                    preferredHeight: 400
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                ListView {
                    id: pasteList
                    objectName: "pasteList"
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
                                                mimeType: "text/url"
                                                data: ListItemData.pasteUrl
                                                invokeActionId: "bb.action.SHARE"
                                            }
                                        }
                                        DeleteActionItem {
                                            title: qsTr("Delete")
                                            imageSource: "asset:///images/action-delete.png"
                                            enabled: false
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
            }
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
