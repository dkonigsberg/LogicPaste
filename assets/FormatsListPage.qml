import bb.cascades 1.0

Page {
    id: formatsListPage
        
    content: Container {
        background: Color.create ("#272727")
        
        PageTitle {
            titleText: "Paste Formats"
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Fill
            }
        }
        
	    ListView {
	        id: formatsList
	        
            dataModel: XmlDataModel {
                id: formatsModel
                source: "models/paste_formats.xml"
            }
	        
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            
            listItemComponents: [
				ListItemComponent {
				    type: "format"
				    StandardListItem {
				        id: formatItem
				        titleText: ListItemData.description
				    }
                }
            ]
            
            onSelectionChanged: {
                nav.pop();
            }
	    }
    }
}
