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
	            
	            listItemComponents: [
	                ListItemComponent {
	                    StandardListItem {
	                        id: pasteItem
	                        titleText: ListItemData.title
	                        descriptionText: ListItemData.pasteDate
	                        statusText: ListItemData.format
	                    }
	                }
	            ]
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
                startRefresh();
            }
        }
    ]
    
    function startRefresh() {
        refreshAction.enabled = false;
        activityIndicator.start();
        pasteListPage.refreshPage();
    }
    
    function refreshComplete() {
        activityIndicator.stop();
        refreshAction.enabled = true;
    }
}
