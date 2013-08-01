import bb.cascades 1.0
import bb.system 1.0

Page {
    id: documentBrowserPage
    
    // the path for browsing. Path must start with '/', with root path is '/'
    // format: /[..]/[..]
    property string documentPath: '/';
    
    Container {
        layout: StackLayout {}
        leftPadding: 24
        rightPadding: 24        
        Container{}
        Container {
            // title Container
            id: titleContainer
            implicitLayoutAnimationsEnabled: false
            topPadding: 7
            bottomPadding: 9
            horizontalAlignment: HorizontalAlignment.Fill
//            Label {
//               implicitLayoutAnimationsEnabled: false
//                visible: true
//                verticalAlignment: VerticalAlignment.Center
//                id: titleLabel
//                text: "WRITER"
//                textStyle.fontWeight: FontWeight.W600
//                horizontalAlignment: HorizontalAlignment.Center
//            }
            TextField {
                id: titleTextField
                enabled: true                
                textStyle.fontWeight: FontWeight.W600
                text: "WRITER"
                implicitLayoutAnimationsEnabled: false
                textStyle.textAlign: TextAlign.Center
                clearButtonVisible: false
                backgroundVisible: false
                focusHighlightEnabled: false
                onTextChanged: {
                    actionUpdateFolderName();
                }
            }
            
        } // end Title Container
        Container {
            //seperator
            background: Color.DarkGray
            horizontalAlignment: HorizontalAlignment.Fill
            preferredHeight: 2
        }
        ListView {
            id: documentListView
            dataModel: fileModels                   
            listItemComponents: [                
                ListItemComponent {
                    Container {                     
                        id: fileComponent
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
                                title: ListItemData.name
                                ActionItem {
                                    title: "Sample Action"   
                                }
                                DeleteActionItem {
                                    title: "Delete"
                                    onTriggered: {
                                        fileComponent.ListItem.view.actionDeleteListItem(fileComponent.ListItem.indexPath);
                                    }
                                }
                            }
                        ] // end of Context Action
                        ListItem.onSelectionChanged: {
                            console.log('ListItem.onSelectionChanged', selected);
                            listItemContainer.background = selected ? Color.create('#ebebeb') : Color.Transparent;
                        }
                        ListItem.onActivationChanged: {
                            var activeChangable = active;
                            var selection = ListItem.view.selectionList();
                            console.log('ListItem.onActivationChanged', active,'indexPath:', ListItem.indexPath, selection.length, 'selections:', selection, 'indexOf', selection.indexOf(ListItem.indexPath), 'selectType',typeof(selection));
                            
                            if (selection.length > 0) {
                                for (var i = 0; i < selection.length; i++) {
                                    console.log('listing selection #',i, ':',selection[i]);
                                    if (selection[i].length > 0 && selection[i][0] == ListItem.indexPath) {
                                        console.log('switch active -> true since the row is selected');
                                        activeChangable = true;
                                    }
                                }
                            }
                            listItemContainer.background = activeChangable ? Color.create('#ebebeb') : Color.Transparent;
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
            multiSelectAction: MultiSelectActionItem {
            }
            multiSelectHandler {
                actions: [
                    DeleteActionItem {
                        onTriggered: {
                            console.log('[documentBrowser-multiSelectHandler]DeleteAction:length:', documentListView.selectionList().length );
                            documentListView.actionDeleteSelectedItems();
                        }
                    }
                ]
                status: 'None selected'                                
            } // end of ListView's multiSelectHandler
                                
            attachedObjects: [
                ArrayDataModel {
                    id: fileModels
                }
            ]
            
            onSelectionChanged: {
                console.log('selectionChanged -> ', indexPath, 'selectionLength', selectionList().length);
                var itemSelected = selectionList().length;
                if (itemSelected == 0) {
                    multiSelectHandler.status = 'None selected';
                }  
                else if (itemSelected == 1) {
                    multiSelectHandler.status = '1 document selected';
                }
                else {
                    multiSelectHandler.status = '' + itemSelected + ' documents selected';
                }
            }
            onTriggered: {
                console.log('[documentBrowser]listViewTriggered', indexPath);
                if (indexPath >= fileModels.size()) return;
                var entry = fileModels.value( indexPath );
                if (entry.type == 'file') {
                    actionOpenFile( entry, {focusEditor:true} );                    
                }
                else if (entry.type == 'folder' ) {
                    actionOpenFolder( entry );
                }
            }

            function actionDeleteSelectedItems() {
                console.log('actionDeleteSelectedItems');
                var selection = selectionList();
                deleteConfirmationDialog.body = 'Delete ' + selection.length + ' selected document' + (selection.length>1?'s':'') + '?';
                deleteConfirmationDialog.exec();
                var status = deleteConfirmationDialog.result;
                if (status == SystemUiResult.ConfirmButtonSelection) {
                    var deleteIndexPaths = [];
                    for (var i = 0 ; i < selection.length;i++) {
                        if (selection[i].length > 0)
                            deleteIndexPaths.push( selection[i][0] );
                    }
                    
                    // reverse sort
                    deleteIndexPaths.sort(function(a,b){return b-a});
                    if (deleteIndexPaths.length > 0) {
                        for (var i = 0; i < deleteIndexPaths.length;i++) {
                            var indexPath = deleteIndexPaths[i];
                            if (indexPath > fileModels.size()) continue;
                            var entry = fileModels.value( indexPath );
                            if (entry.type == 'folder') {
                                //TODO delete
                            }
                            else if (entry.type == 'file') {
                                if (writerApp.deleteFile(entry.path)) 
                                    fileModels.removeAt(indexPath);
                            }
                        }    
                    }
                }
            }
            
            function actionDeleteListItem( indexPath) {
                console.log('actionDeleteListItem:indexPath', indexPath );
                if ( indexPath >= fileModels.size() ) return;
                
                var entry = fileModels.value(indexPath);
                deleteConfirmationDialog.body = entry.type == 'file'? 'Delete the selected document?':'Delete the selected folder?';
                deleteConfirmationDialog.exec();
                var status = deleteConfirmationDialog.result;
                if (status == SystemUiResult.ConfirmButtonSelection) {
                    if (indexPath > fileModels.size()) return;                
                    if (entry.type == 'folder') {
                        if ( writerApp.deleteFolder(entry.path) ) {
                            showMessageToast( "Folder is deleted");
                            fileModels.removeAt(indexPath);
                        }
                    }
                    else if (entry.type == 'file' ) {
                        if ( writerApp.deleteFile(entry.path) ) {
                            showMessageToast( "Document is deleted" );
                            fileModels.removeAt(indexPath);
                        }
                    }
                }
            }
        } // end of ListView
        
    } // end of Root Container
    actions: [
        ActionItem {
            title: "New Document"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/ic_add_file.png"
            onTriggered: {
                actionNewDocument();
            }
        },
        ActionItem {
            title: "New Folder"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/ic_add_folder.png"
            onTriggered: {
                actionNewFolder();
            }
        }
    ]
    
    onCreationCompleted: {
        console.log('[DocumentBrowser]onCreationCompleted:documentPath:', documentPath );
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
        updateTitle();
    }
    
    function updateTitle() {
        var comps = documentPath.split('/');
        if (comps.length > 0) {
            var name = comps[ comps.length - 1 ];
            if (name.length > 0) {
                titleTextField.text = name;
            }
        }
    }

    function actionOpenFolder( folderInfo ) {
        if ( folderInfo.type != 'folder' )
            return;
        console.log('[DocumentBrowser]actionOpenFolder:',folderInfo.path);
        var relativePath = writerApp.relativePath( folderInfo.path );
        console.log('[DocumentBrowser]relativePath',relativePath);
        
        var newBrowser = documentBrowserPageDef.createObject();
        newBrowser.documentPath = relativePath;
        newBrowser.reloadDirectory();

        rootNavigationPane.push(newBrowser);
    }
    
    /**
     * @param    options    :focusEditor
     *                      :focusTitle
     *                      :clearTitle 
     */
    function actionOpenFile(fileInfo, options) {
        if (options === undefined || options === null) 
            options = {};
            
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
        
        if (options.focusEditor) {
            editor.focusEditor();
        }
        else if (options.focusTitle) {
            editor.focusTitle();
        }
        if (options.clearTitle)
            editor.clearTitle();
    }
    
    /*
     * {event_handler}
     */ 
    function onDocumentTitleUpdated( newTitle ) {
        console.log('onDocumentTitleUpdated', newTitle);
        reloadDirectory();
    }
    
    /**
     * Create a new untitled folder
     */     
    function actionNewFolder() {
        var newFolder = writerApp.createEmptyFolder( documentPath );
        console.log('[DocumentBrowser]actionNewFolder', newFolder);
        reloadDirectory();
        
        // open folder
        actionOpenFolder( newFolder );
    }
    
    /**
     * Create a new untitled document and open the editor to edit
     */
    function actionNewDocument() {
        var newFile = writerApp.createEmptyFile( documentPath );
        var isValid = newFile.hasOwnProperty('name');
        console.log('actionNewDocument', isValid );
        if (!isValid) {
            //TODO show messages
            return;
        }
        reloadDirectory();
        actionOpenFile( newFile, {focusTitle:true, clearTitle:true} );
    }

    function actionUpdateFolderName() {
        
    }
    
    function showMessageToast( message ) {
        mainMessageToast.body = message;
        mainMessageToast.show();
    }

    /**
     * QML Page defines glMetaData() method for inter-operating with other QML Objects
     */
    function glMetaData() {
        return {
            pageType: 'documentBrowser'
        };
    }

} // end of Page
