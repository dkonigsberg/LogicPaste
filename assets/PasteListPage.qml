import bb.cascades 1.0

Page {
    id: pasteListPage
    
    signal refreshPage()
    
    content: Container {
        background: Color.create ("#272727")
        
        Container{
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
	            
			    signal openPaste(string pasteUrl)
			    signal openPasteInBrowser(string pasteUrl)
			    signal copyUrl(string pasteUrl)
			    signal deletePaste(string pasteKey)
	            
	            listItemComponents: [
	                ListItemComponent {
	                    StandardListItem {
	                        id: pasteItem
	                        titleText: ListItemData.title
	                        descriptionText: ListItemData.pasteDate
	                        statusText: ListItemData.format
	                        imageSource: ListItemData.imageSource
	                        contextActions: [
	                            ActionSet {
	                                title: "Paste actions"
	                                ActionItem {
	                                    title: "Open"
	                                    onTriggered: {
	                                        pasteItem.ListItem.view.openPaste(ListItemData.pasteUrl);
	                                    }
	                                }
	                                ActionItem {
	                                    title: "Open in browser"
	                                    onTriggered: {
	                                        pasteItem.ListItem.view.openPasteInBrowser(ListItemData.pasteUrl);
	                                    }
	                                }
	                                ActionItem {
	                                    title: "Copy URL"
	                                    onTriggered: {
	                                        pasteItem.ListItem.view.copyUrl(ListItemData.pasteUrl);
	                                    }
	                                }
	                                ActionItem {
	                                    title: "Delete"
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
                    if(selected) {
                        var chosenItem = dataModel.data(indexPath);
                        pasteList.openPaste(chosenItem.pasteUrl)
                    }
                }
	        }
	    }
    }
        
    actions: [
        ActionItem {
            id: refreshAction
            objectName: "refreshAction"
            title: "Refresh"
            imageSource: "asset:///images/icon-refresh-action.png"
            enabled: false
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
