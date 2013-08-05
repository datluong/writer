import bb.cascades 1.0

Sheet {
    id: themePickerSheet
    signal themePicked(string themeName);
    
    Page {
        titleBar: TitleBar {
            title: "Theme"
            visibility: ChromeVisibility.Visible
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    themePickerSheet.close();
                }
            }
        }
        
        Container {
            Container {
                preferredHeight: 0
                horizontalAlignment: HorizontalAlignment.Fill
            }
            ListView {
                id: themeListView
                dataModel: themeModels
                listItemComponents: [
                    ListItemComponent {
                        Container {
                            id: themeItemContainer
                            horizontalAlignment: HorizontalAlignment.Fill
                            Container {
                                enabled: false
                                horizontalAlignment: HorizontalAlignment.Fill
                                preferredHeight: 120
                                background: Color.create( ListItemData.backgroundColor )
                                layout: DockLayout {}                                
                                Container {
                                    id: highlightContainer
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    preferredHeight: 120
                                    layout: DockLayout {}
                                    leftPadding: 24
                                    rightPadding: 24                                    
                                    Label {
                                        textStyle.color: Color.create( ListItemData.textColor );
                                        text: ListItemData.name
                                        horizontalAlignment: HorizontalAlignment.Left
                                        verticalAlignment:  VerticalAlignment.Center
                                    }
                                    ImageView {
                                        id: pickedImageView
                                        imageSource: (ListItemData.type == 'dark' ? "asset:///images/check-light.png" : "asset:///images/check-dark.png")
                                        horizontalAlignment: HorizontalAlignment.Right
                                        verticalAlignment: VerticalAlignment.Center
                                        visible: ListItemData.themeActive
                                    }
                                }
                            }   
                            Divider {
                                topMargin: 0
                                bottomMargin: 0
                            }
                            ListItem.onSelectionChanged: {
                                console.log('ThemeListItem.onSelectionChanged', selected);
                                if (selected) highlightContainer.background = (ListItemData.type == 'dark' ? Color.create(1, 1, 1, 0.15) : Color.create(0, 0, 0, 0.1));
                                else highlightContainer.background = Color.Transparent;
                            }
                            ListItem.onActivationChanged: {
                                console.log('ThemeListItem.onActivationChanged', active);
                                if (active) highlightContainer.background = (ListItemData.type == 'dark' ? Color.create(1, 1, 1, 0.15) : Color.create(0, 0, 0, 0.1));
                                else highlightContainer.background = Color.Transparent;
                            }
                            
                        }
                        
                        
                    } // end of ListItemComponent
                    
                ]
                onTriggered: {
                    console.log('ThemeListItem:triggered');
                    if (indexPath >= themeModels.size())
                        return;
                    var item = themeModels.value(indexPath);
                    
                    themePicked(item.name);                    
                    themePickerSheet.close();
                }
            } // end of ListView
        } // end of main Container
    } // end of Page

    attachedObjects: [
        ArrayDataModel {
            id: themeModels
        }
    ]
    
    onCreationCompleted: {
//        themeModels.append( { name: 'Light', backgroundColor:'#ffffff', textColor: '#000000', type: 'light', themeActive:true } );
//        themeModels.append( { name: 'Dark', backgroundColor:'#000000', textColor: '#ffffff', type: 'dark', themeActive:false } );
//        themeModels.append( { name: 'Sepia', backgroundColor:'#fbf0d9', textColor: '#5f4b32', type: 'light',
//                themeActive: false
//            } );
//        themeModels.append( { name: 'Zenburn', backgroundColor:'#3f3f3f', textColor: '#dcdccc', type: 'dark',
//                themeActive: false
//            } );
        var themes = themeManager.themeList();
        themeModels.append( themes );
    }

} // end of Sheet