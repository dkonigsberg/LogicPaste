import bb.cascades 1.0

Page {
    id: about
    signal close()
    signal openUrl(string url)
    property string appName: "LogicPaste"
    property string versionNumber: "X.Y.Z"
    property string appCopyright: "Copyright \u00A9 2012, Derek Konigsberg"
    titleBar: TitleBar {
        title: "About"
        dismissAction: ActionItem {
            title: "Close"
            onTriggered: {
                about.close();
            }
        }
    }
    content: Container {
        background: Color.create("#f8f8f8")
        layout: DockLayout {
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.TopToBottom
            }
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            ImageView {
                imageSource: "asset:///images/pastebin-logo.png"
                scalingMethod: ScalingMethod.AspectFill
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
            }
            Label {
                text: appName
                topMargin: 25
                bottomMargin: 25
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                textStyle {
                    base: SystemDefaults.TextStyles.TitleText
                }
            }
            Label {
                text: "Version " + versionNumber
                topMargin: 25
                bottomMargin: 25
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                textStyle {
                    base: SystemDefaults.TextStyles.SubtitleText
                }
            }
            Label {
                text: appCopyright
                topMargin: 25
                bottomMargin: 25
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                textStyle {
                    base: SystemDefaults.TextStyles.SmallText
                }
            }
            Divider {
                topMargin: 50
                bottomMargin: 50
            }
            Label {
                topMargin: 30
                bottomMargin: 50
                text: "PASTEBIN | #1 paste tool since 2002"
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                textStyle {
                    base: SystemDefaults.TextStyles.SubtitleText
                    color: Color.DarkBlue
                }
                onTouch: {
                    if (event.isUp()) {
                        about.openUrl("http://pastebin.com");
                    }
                }
            }
            Label {
                topMargin: 50
                bottomMargin: 30
                text: "Paste formatting provided by Pygments"
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                textStyle {
                    base: SystemDefaults.TextStyles.SubtitleText
                    color: Color.DarkBlue
                }
                onTouch: {
                    if (event.isUp()) {
                        about.openUrl("http://pygments.org/");
                    }
                }
            }
        }
    }
}
