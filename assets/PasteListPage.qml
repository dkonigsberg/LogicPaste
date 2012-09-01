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
                    layoutProperties: DockLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
                ListView {
                    id: pasteList
                    objectName: "pasteList"
                    signal openPaste(string pasteUrl, string format)
                    signal openPasteInBrowser(string pasteUrl)
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
                                            enabled: ! ListItemData.isPrivate
                                            onTriggered: {
                                                pasteItem.ListItem.view.openPaste(ListItemData.pasteUrl, ListItemData.format);
                                            }
                                        }
                                        ActionItem {
                                            title: qsTr("Open in browser")
                                            onTriggered: {
                                                pasteItem.ListItem.view.openPasteInBrowser(ListItemData.pasteUrl);
                                            }
                                        }
                                        ActionItem {
                                            title: qsTr("Copy URL")
                                            onTriggered: {
                                                pasteItem.ListItem.view.copyUrl(ListItemData.pasteUrl);
                                            }
                                        }
                                        DeleteActionItem {
                                            title: qsTr("Delete")
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
                    onSelectionChanged: {
                        if (selected) {
                            var chosenItem = dataModel.data(indexPath);
                            if (chosenItem.isPrivate) {
                                pasteList.clearSelection();
                            } else {
                                pasteList.openPaste(chosenItem.pasteUrl, chosenItem.format)
                            }
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
        function resetListFocus() {
            pasteList.clearSelection();
        }
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
    onTopChanged: {
        if (page != mainTab) {
            page.resetListFocus();
        } else if (page == mainTab) {
            if (mainTab.activeTabPane == pasteListPage) {
                mainTab.activeTabPane.resetListFocus();
            }
        }
    }
}
