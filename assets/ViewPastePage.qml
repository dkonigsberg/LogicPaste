import bb.cascades 1.0

Page {
    id: viewPastePage

    content: Container {
        background: Color.create ("#272727")
        layout: StackLayout {
            layoutDirection: LayoutDirection.TopToBottom
        }
        scrollMode: ScrollMode.Both
        WebView {
            id: webView
            objectName: "webView"
            onNavigationRequested: {
                console.debug ("NavigationRequested: " + request.url + " navigationType=" + request.navigationType)
            }
        }
    }
}