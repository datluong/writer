import bb.cascades 1.0

Sheet {
    id: aboutSheet
    Page {
        titleBar: TitleBar {
            title: "About"
            visibility: ChromeVisibility.Visible
            acceptAction: ActionItem {
                title: "Close"
                onTriggered: {
                    aboutSheet.close();
                }
            }
        } // end of TitleBar

        ScrollView {
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                layout: StackLayout {
                }
                topPadding: 64
                ImageView {
                    imageSource: "asset:///images/app-icon.png"
                    horizontalAlignment: HorizontalAlignment.Center
                }

                Label {
                    topMargin: 44
                    bottomMargin: 4
                    text: "Writer for BlackBerry 10"
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: versionLabel
                    topMargin: 0
                    bottomMargin: 4
                    text: "Version 1.0"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText

                }
                Label {
                    topMargin: 0
                    bottomMargin: 4
                    text: "Copyright © 2009-2013 Glam Software"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                Label {
                    topMargin: 0
                    bottomMargin: 44
                    textFormat: TextFormat.Html
                    text: "<a href='mailto:support@glamdevelopment'>support@glamdevelopment.com</a>"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                // other section
                Label {
                    bottomMargin: 4
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.color: Color.Black
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                    textStyle.fontWeight: FontWeight.Bold
                    text: "Other apps for BlackBerry 10"
                }
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    topMargin: 32
                    bottomMargin: 44
//                    ImageView {
//                        imageSource: "asset:///images/slicktasks-app-icon.png"
//                        horizontalAlignment: HorizontalAlignment.Center
//                    }
                    ImageButton {
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/slicktasks-app-icon.png"
                        pressedImageSource: "asset:///images/slicktasks-app-icon-active.png"
                        onClicked: {
                            writerApp.actionOpenBBW('24889874');
                        }
                    }
                }

                // end of other section
                Divider {

                }
                Label {
                    topMargin: 44
                    text: "Found a bug? Have an idea? Please share it with us !"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                    textStyle.color: Color.DarkGray
                }
                Button {
                    topMargin: 44
                    bottomMargin: 44
                    horizontalAlignment: HorizontalAlignment.Center
                    text: "Send Feedback"
                    onClicked: {
                        writerApp.actionSendFeedback();
                    }
                }
                Container {
                }
            } // end of Main Container
        } // end of ScrollView
    } // end of Page
    onCreationCompleted: {
        versionLabel.setText("Version " + writerApp.appVersion());
    }

}// end of Sheet
