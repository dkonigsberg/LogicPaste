import bb.cascades 1.0

Page {
    id: viewPastePage
    content: Container {
        layout: DockLayout {
        }
        ActivityIndicator {
            id: activityIndicator
            objectName: "activityIndicator"
            preferredWidth: 400
            preferredHeight: 400
            layoutProperties: DockLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
        }
        ScrollView {
            scrollViewProperties {
                scrollMode: ScrollMode.Vertical
            }
            layoutProperties: DockLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
            }
            WebView {
                id: webView
                objectName: "webView"
                onNavigationRequested: {
                    console.debug("NavigationRequested: " + request.url + " navigationType=" + request.navigationType)
                }
                onLoadingChanged: {
                    if (loadRequest.status == WebView.LoadSucceededStatus) {
                        activityIndicator.stop();
                    }
                    if (loadRequest.status == WebView.LoadFailedStatus) {
                        activityIndicator.stop();
                    }
                }
            }
        }
    }
    onCreationCompleted: {
        activityIndicator.start();
    }
}
