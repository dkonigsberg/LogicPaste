import bb.cascades 1.0

TabbedPane {
    id: mainTab
    showTabsOnActionBar: true
    Tab {
        title: "Paste"
        imageSource: "asset:///images/icon-paste-tab.png"
        PastePage {
            id: pastePage
            objectName: "pastePage"
        }
    }
    Tab {
        title: "My Pastes"
        imageSource: "asset:///images/icon-history-tab.png"
        PasteListPage {
            id: historyPage
            objectName: "historyPage"
        }
    }
    Tab {
        title: "Trending"
        imageSource: "asset:///images/icon-trending-tab.png"
        PasteListPage {
            id: trendingPage
            objectName: "trendingPage"
        }
    }
    Tab {
        title: "Settings"
        imageSource: "asset:///images/icon-settings-tab.png"
        SettingsPage {
            id: settingsPage
            objectName: "settingsPage"
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
