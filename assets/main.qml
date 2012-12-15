import bb.cascades 1.0

TabbedPane {
    id: mainTab
    showTabsOnActionBar: true
    property alias historyPlaceholderText: historyPage.placeholderText
    property alias historyPlaceholderImageSource: historyPage.placeholderImageSource
    Tab {
        title: qsTr("Paste")
        imageSource: "asset:///images/icon-paste-tab.png"
        PastePage {
            id: pastePage
            objectName: "pastePage"
        }
    }
    Tab {
        title: qsTr("My Pastes")
        imageSource: "asset:///images/icon-history-tab.png"
        PasteListPage {
            id: historyPage
            objectName: "historyPage"
            deleteEnabled: true
        }
    }
    Tab {
        title: qsTr("Trending")
        imageSource: "asset:///images/icon-trending-tab.png"
        PasteListPage {
            id: trendingPage
            objectName: "trendingPage"
            placeholderImageSource: "asset:///images/placeholder-refresh.png"
            placeholderText: qsTr("Tap on the refresh action to load trending pastes")
        }
    }
    Tab {
        title: qsTr("Settings")
        imageSource: "asset:///images/icon-settings-tab.png"
        SettingsPage {
            id: settingsPage
            objectName: "settingsPage"
        }
    }
    onCreationCompleted: {
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.All;
        cs.settingsUpdated.connect(onSettingsUpdated);
        onSettingsUpdated();
    }
    function onSettingsUpdated() {
        settingsPage.onSettingsUpdated();
    }
}
