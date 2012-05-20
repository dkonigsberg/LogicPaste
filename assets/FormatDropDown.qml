import bb.cascades 1.0

DropDown {
    title: "Paste format"
    Option {
        selected: true
        text: "None"
        value: "text"
    }
    Option {
        text: "More"
        
        onSelectedChanged : {
            if (selected == true) {
                nav.deprecatedPushQmlByString("FormatsListPage.qml");
           }
        }
    }
}
