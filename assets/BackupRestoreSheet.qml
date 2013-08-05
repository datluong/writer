import bb.cascades 1.0

Sheet {
    id: backupRestoreSheet
    signal backup();
    signal restore();
    
    Page {
        titleBar: TitleBar {
            title: "Backup/Restore"
            visibility: ChromeVisibility.Visible
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    backupRestoreSheet.close();
                }
            }
        }
                
        Container {
            Container {}
            Container {
                leftPadding: 24
                rightPadding: 24
                topPadding: 44
                bottomPadding: 28
                horizontalAlignment: HorizontalAlignment.Fill
                Button {
                    horizontalAlignment: HorizontalAlignment.Fill
                    text: "Backup your writings"
                    onClicked: {
                        backup();
                        backupRestoreSheet.close();
                    }
                }
                Label {
                    topMargin: 12
                    horizontalAlignment: HorizontalAlignment.Left
                    text: "This will create a backup file, containing all of your writings and save it to a specified backup destination."
                    multiline: true
                    textStyle.fontSize: FontSize.XSmall
                    textStyle.color: Color.DarkGray
                }
            } // end of back up Container
            Container {
                visible: true
                leftPadding: 24
                rightPadding: 24
                bottomPadding: 28
                horizontalAlignment: HorizontalAlignment.Fill
                Button {
                    horizontalAlignment: HorizontalAlignment.Fill
                    text: "Restore your writings"
                    onClicked: {
                        restore();
                        backupRestoreSheet.close();
                    }
                }
                Label {
                    topMargin: 12
                    horizontalAlignment: HorizontalAlignment.Center
                    multiline: true
                    text: "Restore your writings from a backup file."
                    textStyle.fontSize: FontSize.XSmall
                    textStyle.color: Color.DarkGray
                }
            } // restore Container

        } // end of Root Container
    } // end of Page

} // end of Sheet
