import bb.cascades 1.0

Page {
    id: viewPastePage
    content: ScrollView {
        scrollViewProperties {
            scrollMode: ScrollMode.Vertical
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.TopToBottom
            }
            WebView {
                id: webView
                objectName: "webView"
                onNavigationRequested: {
                    console.debug("NavigationRequested: " + request.url + " navigationType=" + request.navigationType)
                }
            }
        }
    }
}
