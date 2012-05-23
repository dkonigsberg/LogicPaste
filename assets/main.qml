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
        
        PasteListPage {
            id: historyPage
            objectName: "historyPage"
            paneProperties: TabbedPaneProperties {
                title: "My Pastes"
                imageSource: "asset:///images/icon-history-tab.png"
            }
        }
        
        PasteListPage {
            id: trendingPage
            objectName: "trendingPage"
            paneProperties: TabbedPaneProperties {
                title: "Trending"
                imageSource: "asset:///images/icon-trending-tab.png"
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
    
    onTopChanged: {
        if (pane == historyPage || pane == trendingPage) {
            pane.resetListFocus();
        }
        else if(pane == mainTab) {
            if(mainTab.activeTabPane == historyPage || mainTab.activeTabPane == trendingPage) {
                mainTab.activeTabPane.resetListFocus();
            }
        }
    }
    
    onCreationCompleted: {
        cs.settingsUpdated.connect(onSettingsUpdated);
        onSettingsUpdated();
    }
    
    function onSettingsUpdated() {
        settingsPage.onSettingsUpdated();
    }
}
