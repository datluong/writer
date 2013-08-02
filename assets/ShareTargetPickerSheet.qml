import bb.cascades 1.0
import bb.system 1.0

Sheet {
    id: shareTargetPickerSheet
    signal shareTargetPicked(variant type);
    
    Page {
        titleBar: TitleBar {
            title: "Share"
            visibility: ChromeVisibility.Visible
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    shareTargetPickerSheet.close();
                }
            }
        }
        Container {
            Container {
                preferredHeight: 0
                horizontalAlignment: HorizontalAlignment.Fill
            }
            ListView {
                dataModel: shareModels
                topPadding: 12
                listItemComponents: [
                    ListItemComponent {
                        StandardListItem {
                            title: ListItemData.title
                            imageSource: ListItemData.imageSource
                        }
                    }
                ]
                onTriggered: {
                    console.log('list view triggered:', indexPath);
                    if (indexPath < 0 || indexPath >= shareModels.size()) return;
                    shareTargetPicked( shareModels.value(indexPath) );
                    shareTargetPickerSheet.close();
                }
            } // end of ListView
        } // end of Main Containre
    } // end of Page

    attachedObjects: [
        ArrayDataModel {
            id: shareModels
            objectName: "shareModels"
        }
    ]

}// end of Sheet
