import bb.cascades 1.0

Page {
    property string documentPath: '';
    
    Container {
        layout: StackLayout {}
        leftPadding: 24
        rightPadding: 24        
        Container{}
        Container {
            // title Container            
            topPadding: 24
            bottomPadding: 28
            horizontalAlignment: HorizontalAlignment.Fill
            Label {
                id: titleLabel
                text: "WRITER"
                textStyle.fontWeight: FontWeight.W600
                horizontalAlignment: HorizontalAlignment.Center
            }            
            
        } // end Title Container
        Container {
            //seperator
            background: Color.DarkGray
            horizontalAlignment: HorizontalAlignment.Fill
            preferredHeight: 2
        }
        ListView {
            dataModel: fileModels
            onSelectionChanged: {
                console.log('selectionChanged -> ', indexPath);
            }            
            listItemComponents: [                
                ListItemComponent {                    
                    Container {                        
                        Container {
                            property int containterWith: 0
                            id: listItemContainer
                            preferredHeight: 94
                            leftPadding: 24
                            horizontalAlignment: HorizontalAlignment.Fill
                            layout: DockLayout {}
                            Label {
                                id: documentNameLabel
                                text: ListItemData.name
                                textStyle.fontWeight: FontWeight.W200
                                verticalAlignment: VerticalAlignment.Center
                            }
                            Label {
                                text: "Folder"
                                visible: (ListItemData.type == 'folder')
                                horizontalAlignment: HorizontalAlignment.Right
                                verticalAlignment: VerticalAlignment.Center
                                textStyle.fontWeight: FontWeight.W500
                                textStyle.fontSize: FontSize.XXSmall
                                attachedObjects: [
                                    LayoutUpdateHandler {
                                        onLayoutFrameChanged: {                                            
                                            console.log('[folderIndicator]onLayoutFrameChanged', layoutFrame.width, 'containerWidth', listItemContainer.containterWith);
                                            documentNameLabel.setMaxWidth( listItemContainer.containterWith - layoutFrame.width - 36 );
                                        }
                                    }
                                ]
                            }                            
                        }
                        Divider {
                            topMargin: 0
                            bottomMargin: 0
                        }
                        contextActions: [
                            ActionSet {
                                ActionItem {
                                    title: "Sample Action"   
                                }
                            }
                        ] // end of Context Action
                        ListItem.onSelectionChanged: {
                            listItemContainer.background = selected ? Color.create('#ebebeb') : Color.Transparent;
                        }
                        ListItem.onActivationChanged: {
                            listItemContainer.background = active ? Color.create('#ebebeb') : Color.Transparent;
                        }
                        attachedObjects: [
                            LayoutUpdateHandler {
                                onLayoutFrameChanged: {
                                    listItemContainer.containterWith = layoutFrame.width;
                                }
                            }
                        ]
                    }
                    
                } // end ListItemComponent Definition
            ]
            attachedObjects: [
                ArrayDataModel {
                    id: fileModels
                }
            ]
            onTriggered: {
                console.log('[documentBrowser]listViewTriggered', indexPath);
                if (indexPath >= fileModels.size()) return;
                var entry = fileModels.value( indexPath );
                if (entry.type == 'file') {
                    actionOpenFile(entry);                    
                }
            }
        } // end of ListView
        
    } // end of Root Container
    actions: [
        ActionItem {
            title: "Document"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/ic_add_file.png"
            onTriggered: {
                actionNewDocument();
            }
        },
        ActionItem {
            title: "Folder"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/ic_add_folder.png"
        }
    ]
    
    onCreationCompleted: {
        console.log('onCreationCompleted:documentPath:', documentPath );
//        fileModels.append({
//                name: 'My random thoughts asdsadlkjsa dlkasjdlsakjdaslkd aslkjdaslkdj asl dalskjdaslkdj asd laskjdslakjda',
//                type: 'folder'
//            });
//        fileModels.append({
//                name: 'My random thoughts',
//                type: 'folder'
//            });
//        fileModels.append({
//                name: 'Hello',
//                type: 'file'
//            });
        reloadDirectory();    
    }
    
    function reloadDirectory() {
        fileModels.clear();
        var itemData = writerApp.listDirectory(documentPath);
        fileModels.append(itemData);
    }
    
    function actionOpenFile(fileInfo) {
        console.log('[Document Browser]:actionOpenFile', fileInfo);
        var filePath = fileInfo.path;
        if ( !writerApp.isFileLoadable(filePath) ) {
            //TODO show error message
            return;
        }
        
        var text = writerApp.loadFileContent( filePath );
        console.log('contentInfo', text.length, 'type', typeof(text) );
        if (text == '' || text.length == 0 || typeof(text) != 'string' )
            text = '';
        
        // load the file and push the editor screen to view
        var editor = editorPageDef.createObject();
        editor.documentTitle = fileInfo.name;
        editor.documentPath  = fileInfo.path;
        editor.setEditorBodyContent( text );
        editor.documentTitleUpdated.connect( onDocumentTitleUpdated );
        editor.beginEditing();
        
        rootNavigationPane.push(editor);
    }
    
    /*
     * {event_handler}
     */ 
    function onDocumentTitleUpdated( newTitle ) {
        console.log('onDocumentTitleUpdated', newTitle);
        reloadDirectory();
    }
    
    /**
     * Create a new untitled document and open the editor to edit
     */
    function actionNewDocument() {
        var newTitle = writerApp.createEmptyFile( documentPath );        
        console.log('actionNewDocument', newTitle);
        if (newTitle.length == 0) {
            //TODO show message
            return;
        }
        reloadDirectory();
    }
} // end of Page
