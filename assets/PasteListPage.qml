import bb.cascades 1.0

Page {
    id: history
    content: Container {
        background: Color.create ("#272727")
        
        ListView {
            id: historyList
            
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
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
            }
        }
    ]
}
