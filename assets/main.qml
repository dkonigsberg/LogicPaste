import bb.cascades 1.0

NavigationPane {
    id: nav
    TabbedPane {
        id: mainTab
        
        PastePage {
            id: pastePage
            objectName: "pastePage"
            paneProperties: TabbedPaneProperties {
                title: "Paste"
                imageSource: "asset:///images/icon-paste-tab.png"
            }
        }
        
        HistoryPage {
            id: historyPage
            objectName: "historyPage"
            paneProperties: TabbedPaneProperties {
                title: "History"
                imageSource: "asset:///images/icon-history-tab.png"
            }
        }
        
        SettingsPage {
            id: settingsPage
            objectName: "settingsPage"
            paneProperties: TabbedPaneProperties {
                title: "Settings"
                imageSource: "asset:///images/icon-settings-tab.png"
            }
        }
    }
}
