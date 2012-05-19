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
}
