import bb.cascades 1.0

Page {
    id: about
    signal close()
    signal openUrl(string urlString)
    property string appName: "LogicPaste"
    property string versionNumber: "X.Y.Z"
    property string appCopyright: "Copyright \u00A9 2012, Derek Konigsberg\nAll Rights Reserved"
    property bool darkTheme: (Application.themeSupport.theme.colorTheme.style == VisualStyle.Dark)
    titleBar: TitleBar {
        title: qsTr("About")
        dismissAction: ActionItem {
            title: qsTr("Close")
            onTriggered: {
                about.close();
            }
        }
    }
    content: Container {
        layout: DockLayout {
        }
        ScrollView {
            scrollViewProperties {
                scrollMode: ScrollMode.Vertical
            }
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            
            Container {
                topPadding: 50
                bottomPadding: 50
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }
                horizontalAlignment: HorizontalAlignment.Center
                ImageView {
                    imageSource: "asset:///images/pastebin-logo.png"
                    scalingMethod: ScalingMethod.AspectFill
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    text: appName
                    topMargin: 25
                    bottomMargin: 25
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle {
                        base: SystemDefaults.TextStyles.TitleText
                    }
                }
                Label {
                    text: qsTr("Version %1").arg(versionNumber)
                    topMargin: 25
                    bottomMargin: 25
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle {
                        base: SystemDefaults.TextStyles.SubtitleText
                    }
                }
                Label {
                    text: appCopyright
                    topMargin: 25
                    bottomMargin: 25
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        textAlign: TextAlign.Center
                    }
                }
                Divider {
                    topMargin: 50
                    bottomMargin: 50
                }
                Label {
                    topMargin: 50
                    bottomMargin: 50
                    text: "PASTEBIN | #1 paste tool since 2002"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle {
                        base: SystemDefaults.TextStyles.SubtitleText
                        color: darkTheme ? Color.Blue : Color.DarkBlue
                    }
                    onTouch: {
                        if (event.isUp()) {
                            about.openUrl("http://pastebin.com");
                        }
                    }
                }
                Label {
                    topMargin: 50
                    bottomMargin: 50
                    text: qsTr("Icons from the Tango Desktop Project")
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle {
                        base: SystemDefaults.TextStyles.SubtitleText
                        color: darkTheme ? Color.Blue : Color.DarkBlue
                    }
                    onTouch: {
                        if (event.isUp()) {
                            about.openUrl("http://tango.freedesktop.org/");
                        }
                    }
                }
                Label {
                    topMargin: 50
                    bottomMargin: 50
                    text: qsTr("Paste formatting provided by Pygments")
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle {
                        base: SystemDefaults.TextStyles.SubtitleText
                        color: darkTheme ? Color.Blue : Color.DarkBlue
                    }
                    onTouch: {
                        if (event.isUp()) {
                            about.openUrl("http://pygments.org/");
                        }
                    }
                }
                ImageView {
                    topMargin: 50
                    bottomMargin: 50
                    imageSource: "asset:///images/osbbx-logo.png"
                    horizontalAlignment: HorizontalAlignment.Center
                    onTouch: {
                        if (event.isUp()) {
                            about.openUrl("http://www.opensourcebb.com/osbbx/");
                        }
                    }
                }
            }
        }
        ImageView {
            imageSource: "asset:///images/github-forkme.png"
            verticalAlignment: VerticalAlignment.Top
            horizontalAlignment: HorizontalAlignment.Right
            onTouch: {
                if (event.isUp()) {
                    about.openUrl("https://github.com/dkonigsberg/LogicPaste");
                }
            }
        }
    }
}
