import bb.cascades 1.0
import bb.system 1.0

import 'js/wordcounter.js' as WordCounter
import 'js/timeassist.js'  as TimeAssist

Page {
    id: documentBrowserPage
    objectName: "documentBrowserPage"
    // the path for browsing. Path must start with '/', with root path is '/'
    // format: /[..]/[..]
    property bool initialized_: false;
    property string documentPath: '/';
    property bool   documentBrowserInitialized: false;
    property string themeRowTextColor: '';
    property string rowDividerColor: '';
    property string rowHighlightColor: '';
    property variant themePickerSheet;
    property variant backupRestoreSheet;
    
    signal folderNameChanged();
    
    Container {
        id: pageRootContainer
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
                enabled: false            
                textStyle.fontWeight: FontWeight.W600
                text: "WRITER"
                implicitLayoutAnimationsEnabled: false
//                textStyle.textAlign: TextAlign.Center
                clearButtonVisible: false
                backgroundVisible: false
                focusHighlightEnabled: false
                onTextChanged: {
                    if (documentBrowserInitialized) {
                        actionUpdateFolderName();
                        // hide the virtual keyboard
                        if (!writerApp.isPhysicalKeyboardDevice())
                            titleContainer.requestFocus();
                    }
                }
            }
            TextField {
                // this dummy textfield will make titleTextField loses focus on ENTER key
                id: assistTextField                
                visible: false                
            }
        } // end Title Container
        Container {
            //seperator
            id: mainDivider
            background: Color.DarkGray
            horizontalAlignment: HorizontalAlignment.Fill
            preferredHeight: 2
        }
        //
        Container {
            id: sortOptionContainer
            topPadding: 12
            bottomPadding: 8
            visible: false
            DropDown {
                id: sortTypeDropdown
                title: "Sort"
                Option {
                    text: "Name"
                    value: "name"
                }
                Option {
                    text: "Modified Date"
                    value: "modifiedDate"
                }
                onSelectedValueChanged: {
                    console.log('[sortTypeDropdown]selectedValueChanged -> ', selectedValue);
                    if (initialized_) {
                        writerApp.setBrowserSortType( selectedValue );
                        reloadDirectory();
                    }
                    sortOptionContainer.visible = false;
                }
            }
        }
        // move controller
        Container {
            id: fileMoverContainer
            horizontalAlignment: HorizontalAlignment.Fill
            topPadding: 8           
            visible: false
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout {}
                Button {
                    text: "Cancel"
                    horizontalAlignment: HorizontalAlignment.Left
                    verticalAlignment: VerticalAlignment.Center
                    preferredWidth: 120
                    onClicked: {
                        writerApp.clearClipboard();
                    }
                    
                }
                Label {
                    id: moveDescriptionLabel
                    text: "0 item selected"
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                Button {
                    text: "Move"
                    id: moveButton
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Center
                    preferredWidth: 120
                    onClicked: {
                        writerApp.moveToFolder( documentPath );
                    }
                }
            }
            Container { //divider
                topMargin: 8
                bottomMargin: 0
                preferredHeight: 2
                horizontalAlignment: HorizontalAlignment.Fill                
                background: documentListView.themedDividerColor();
            }

        }
        Container {
            id: folderEmptyContainer
            visible: false  
            horizontalAlignment: HorizontalAlignment.Fill
            topPadding: 32
            Label {
                id: folderEmptyLabel
                text: "No documents"
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.fontWeight: FontWeight.W200
            }
        }
        // listview
        ListView {
            id: documentListView
            dataModel: fileModels                   
            listItemComponents: [                
                ListItemComponent {                    
                    Container {
//                        preferredWidth: ListItem.view.determineFullWidth();
                        preferredWidth: 2000
                        function themedRowTextColor_() {
                            return ListItem.view.themedRowTextColor();
                        }

                        function themedDividerColor_() {
                            return ListItem.view.themedDividerColor();
                        }

                        function themedRowHighlightColor_() {
                            return ListItem.view.themedRowHighlightColor();
                        } 
                        
                        function descriptionForFileInfo(fileInfo) {
                            if (fileInfo.hasOwnProperty('description')) {
                                console.log('descriptionForFileInfo:using:description');
                                return fileInfo.description;
                            }
                            if (fileInfo.hasOwnProperty('modified')) {
                                console.log('descriptionForFileInfo:using:measureDistance()');
                                var measure = TimeAssist.measureDistance( fileInfo.modified );
                                return measure.description;
                            }
                        }                        
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
                                textStyle.color: themedRowTextColor_();
                                verticalAlignment: VerticalAlignment.Center
                                
                            }
                            Label {
                                text: ((ListItemData.type == 'folder') ? "Folder" : descriptionForFileInfo(ListItemData) )
                                visible: true
                                horizontalAlignment: HorizontalAlignment.Right
                                verticalAlignment: VerticalAlignment.Center
                                textStyle.fontWeight: (ListItemData.type == 'folder' ? FontWeight.W500 : FontWeight.W200)
                                textStyle.fontSize: FontSize.XXSmall
                                textStyle.color: themedRowTextColor_()
                                attachedObjects: [
                                    LayoutUpdateHandler {
                                        onLayoutFrameChanged: {                                                    
                                            console.log('[folderIndicator]onLayoutFrameChanged', layoutFrame.width, 'containerWidth', listItemContainer.containterWith);
                                            documentNameLabel.setMaxWidth( listItemContainer.containterWith - layoutFrame.width - 36 );
                                        }
                                    }
                                ]
                            }                    
                        } // end ListItem's main Container

                        Container { //divider
                            topMargin: 0
                            bottomMargin: 0
                            preferredHeight: 2
                            horizontalAlignment: HorizontalAlignment.Fill
//                            background: Color.create('#e4e4e4');
                            background: themedDividerColor_();
                        }
                        
                        contextActions: [                            
                            ActionSet {
                                id: documentActionSet
                                title: ListItemData.name
                                subtitle: ( ListItemData.type == 'folder' ? 'Folder' : '');
                                ActionItem {
                                    title: "Move"
                                    imageSource: "asset:///images/icon-file-move.png"
                                    onTriggered: {
                                        fileComponent.ListItem.view.actionMoveListItem(fileComponent.ListItem.indexPath);
                                    }
                                }
                                ActionItem {
                                    title: "Share"
                                    enabled: ( ListItemData.type != 'folder' )
                                    imageSource: "asset:///images/ic_share.png"
                                    onTriggered: {
                                        fileComponent.ListItem.view.actionShareListItem(fileComponent.ListItem.indexPath);
                                    }
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
                            listItemContainer.background = selected ? themedRowHighlightColor_() : Color.Transparent;
                        }
                        ListItem.onActivationChanged: {
                            var activeChangable = active;
                            var selection = ListItem.view.selectionList();
                            console.log('ListItem.onActivationChanged', active,'indexPath:', ListItem.indexPath, 'selectionLength:',selection.length, 'selections:', selection, 'indexOf', selection.indexOf(ListItem.indexPath), 'selectType',typeof(selection));
                            
                            if (selection.length > 0) {
                                for (var i = 0; i < selection.length; i++) {
                                    console.log('listing selection #',i, ':',selection[i]);
                                    if (selection[i].length > 0 && selection[i][0] == ListItem.indexPath) {
                                        console.log('switch active -> true since the row is selected');
                                        activeChangable = true;
                                    }
                                }
                            }
                            else {
                                // single selection
                                if (ListItemData.type == 'file' ) {
                                    // count the number of word
                                    var content = ListItem.view.loadFileContent_( ListItemData.path );
                                    var wc = WordCounter.wordCount( content );
                                    documentActionSet.subtitle = '' + wc + (wc <= 1 ? ' word' : ' words');
                                }
                            }
                            listItemContainer.background = activeChangable ? themedRowHighlightColor_() : Color.Transparent;
                        }
                        attachedObjects: [
                            LayoutUpdateHandler {
                                onLayoutFrameChanged: {
                                    listItemContainer.containterWith = layoutFrame.width;
                                }
                            }
                        ]                        
                    }  // end ListItem's mainContainer                  
                } // end ListItemComponent Definition
            ]
            multiSelectAction: MultiSelectActionItem {
            }
            multiSelectHandler {
                actions: [
                    ActionItem {
                        title: "Move"
                        imageSource: "asset:///images/icon-file-move.png"
                        onTriggered: {
                            documentListView.actionMoveSelectedItems();
                        }
                    },
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
                var stat = statsForSelectedIndexes();
                if (itemSelected == 0) {
                    multiSelectHandler.status = 'None selected';
                }  
                else if (itemSelected == 1) {                    
                    multiSelectHandler.status = (stat.documents > 0 ? '1 document selected' : '1 folder selected');
                }
                else {
                    var type = 'documents';
                    if ( stat.folders > 0 && stat.documents > 0) 
                        type = 'items';
                    if ( stat.documents == 0 )
                        type = 'folders';
                        
                    multiSelectHandler.status = '' + itemSelected + ' ' + type + ' selected';
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
                    actionOpenFolder( entry, {} );
                }
            }
            
            /**
             * @return a list of indexPath when the list is in multi-select mode
             */
            function selectedIndexPaths() {
                var list = [];
                var selection = selectionList();
                for (var i = 0; i < selection.length; i ++) {
                    if (selection[i].length > 0) list.push(selection[i][0]);
                }
                return list;
            }
            
            /**
             * Count the number of selected folders and documents when the list is in multi-select mode 
             */
            function statsForSelectedIndexes() {
                var indexes = selectedIndexPaths();
                var documentCount = 0;
                var folderCount = 0;
                for (var i = 0; i < indexes.length; i++) {
                    var entry = fileModels.value( indexes[i] );
                    if (entry.type == 'folder') 
                        folderCount++;
                    else documentCount ++;
                }
                return { 
                    documents: documentCount,
                    folders: folderCount
                }
            }

            function actionMoveSelectedItems() {
                console.log('[DocumentBrowser]actionMoveSelectedItems');
                var selection = selectionList();
                var fileList = [];
                for (var i = 0; i < selection.length; i ++) {
                    if (selection[i].length > 0) {
                        var indexPath = selection[i][0];
                        var entry = fileModels.value(indexPath);
                        fileList.push( entry.path );
                    }
                }
                
                if (fileList.length > 0) {
                    writerApp.registerClipboard(documentPath, fileList);
                }
            }

            function actionDeleteSelectedItems() {
                console.log('actionDeleteSelectedItems');
                var selection = selectionList();
                deleteConfirmationDialog.body = 'Delete ' + selection.length + ' selected item' + (selection.length>1?'s':'') + '?';
                var successMessage = '' + selection.length + ' item' + (selection.length > 1 ? 's' : '') + ' deleted';
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
                                if (writerApp.deleteFolder(entry.path))
                                    fileModels.removeAt(indexPath);
                            }
                            else if (entry.type == 'file') {
                                if (writerApp.deleteFile(entry.path)) 
                                    fileModels.removeAt(indexPath);
                            }
                        }    
                    }
                }
                updateFolderEmptyIndicator();

                showMessageToast( successMessage );
            }
            
            function actionMoveListItem( indexPath ) {                
                console.log('actionMoveListItem', indexPath);                                
                if (indexPath >= fileModels.size()) return;

                var entry = fileModels.value(indexPath);
                var list = [];
                list.push( entry.path );
                writerApp.registerClipboard( documentPath, list );
            }
            
            function actionShareListItem( indexPath ) {
                console.log('actionShareListItem:indexPath', indexPath);
                if ( indexPath >= fileModels.size() ) return;
                
                var entry = fileModels.value( indexPath );
                if ( entry.type != 'file') return;
                if ( !writerApp.isFileLoadable( entry.path ) ) return;
                
                var content = writerApp.loadFileContent( entry.path );
                writerApp.actionShareDocumentAsAttachment( entry.name, content );
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
                updateFolderEmptyIndicator();
            }        
            
            /**
             * Wrapper method
             */
            function loadFileContent_(path) {
                return writerApp.loadFileContent(path);
            }
            
            function determineFullWidth() {
                return writerApp.displayWidthForCurrentOrientation();
            }
            
            function themedRowTextColor() {
                console.log('themedRowTextColor');
                if (themeRowTextColor.length > 0) {
                    return Color.create(themeRowTextColor);
                }
                else
                    return null;
            }
            
            function themedDividerColor() {
                console.log('themedDividerColor');
                if (rowDividerColor.length > 0)
                    return Color.create(rowDividerColor);
                else
                    return Color.create('#e4e4e4');
            }
            
            function themedRowHighlightColor() {
                console.log('themedRowHighlightColor');
                if (rowHighlightColor.length > 0)
                    return Color.create(rowHighlightColor);
                else
                    return Color.create('#ebebeb'); 
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
        },
        ActionItem {
            id: renameFolderActionItem
            title: "Rename Folder"
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/ic_rename.png"
            onTriggered: {
                titleTextField.editor.setSelection(0, titleTextField.text.length);
                titleTextField.requestFocus();
            }
        },
        ActionItem {
            title: "Sort"
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/icon-sort.png"
            onTriggered: {
                sortOptionContainer.visible = true;
            }
        },
        ActionItem {
            title: "Theme"
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/icon-themes.png"
            onTriggered: {
                actionPickTheme();
            }
        },
        ActionItem {
            title: "Backup/Restore"            
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/icon-backup.png"
            onTriggered: {
                actionBackupRestore();
            }
        }
    ]
    
    onCreationCompleted: {
        console.log('[DocumentBrowser]onCreationCompleted:documentPath:', documentPath );
//        console.log( TimeAssist.measureDistance( new Date(2010,2,3) ).description );
//        console.log( TimeAssist.measureDistance( new Date(2015,2,3) ).description );
        
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
//            }); return;
        reloadDirectory();
        
        var lastEditedDocumentInfo = writerApp.lastDocumentInEditing();
        if (lastEditedDocumentInfo.hasOwnProperty('path')) {
            actionOpenFile( lastEditedDocumentInfo, {focusEditor:true});   
        }
              
        // update sort type
        var sortType = writerApp.browserSortType();
        sortTypeDropdown.setSelectedIndex( sortType == 'name' ? 0 : 1 );
        initialized_ =  true;
    }
    
    function disableRenaming() {
        documentBrowserPage.removeAction( renameFolderActionItem );
    }
    
    function reloadDirectory() {
        fileModels.clear();
        var itemData = writerApp.listDirectory(documentPath);
        fileModels.append(itemData);
        updateTitle();
        
        updateFolderEmptyIndicator();
    }
    
    /**
     * Bind documentPath to title field
     **/
    function updateTitle() {
        var comps = documentPath.split('/');
        if (comps.length > 0) {
            var name = comps[ comps.length - 1 ];
            if (name.length > 0) {
                titleTextField.text = name;
            }
        }
    }

    function actionOpenFolder( folderInfo, options ) {
        if ( folderInfo.type != 'folder' )
            return;
        if (options === null || options === undefined)
            options = {};
            
        console.log('[DocumentBrowser]actionOpenFolder:',folderInfo.path);
        var relativePath = writerApp.relativePath( folderInfo.path );
        console.log('[DocumentBrowser]relativePath',relativePath);
        
        var newBrowser = documentBrowserPageDef.createObject();
        newBrowser.documentPath = relativePath;
        newBrowser.reloadDirectory();
        newBrowser.updateFileMover();
        newBrowser.enableTitleEditing();
        
        newBrowser.folderNameChanged.connect(onSubfolderNameChanged);
        
        newBrowser.applyCustomTheme( themeManager.currentTheme() );
        
        rootNavigationPane.push(newBrowser);
        newBrowser.documentBrowserInitialized = true;
        
        if (options.editFolderName == true) {
            newBrowser.beginEditFolder();
        }
    }
    
    function enableTitleEditing() {
        titleTextField.enabled = true;
    }
    
    function beginEditFolder() {
        titleTextField.editor.setSelection( 0, titleTextField.text.length );
        titleTextField.requestFocus();
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
        editor.documentUpdated.connect( onDocumentUpdated );
        editor.beginEditing();
        
        editor.applyCustomTheme(themeManager.currentTheme());
        rootNavigationPane.push(editor);
        
        writerApp.registerDocumentInEditing( filePath );

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
        reloadDirectory();
    }
    
    /**
     * {event_handler}
     */ 
    function onDocumentUpdated( path ) {
        
        for (var i = 0 ; i < fileModels.size(); i++) {
            var entry = fileModels.value(i);
            if (entry.path == path) {
                var updatedEntry = writerApp.getFileInfo(path);
                if (updatedEntry.hasOwnProperty('path')) {
                    var measure = TimeAssist.measureDistance( updatedEntry.modified );
                    updatedEntry.description = measure.description;
                    updatedEntry.nextRefresh = measure.nextRefresh;
                                                        
                    fileModels.replace(i, updatedEntry);
                    
                    // if the index of the item changed, move it up                    
                    if (writerApp.browserSortType() != 'name') {
                        var newPos = -1;
                        var itemData = writerApp.listDirectory(documentPath);
                        for (var j = 0; j < itemData.length; j++)
                            if (itemData[j].path == updatedEntry.path) {
                                newPos = j; break;
                            }
                        if (newPos != -1 && newPos != i) {
                            fileModels.move(i, newPos);           
                        }
                    }
                }
                
                break;
            }
        }
    }

    function onSubfolderNameChanged() {
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
        actionOpenFolder( newFolder, {editFolderName:true} );
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

    /**
     * {event_handler}
     * Fired when the titlebar's text is changed
     */
    function actionUpdateFolderName() {
        if (documentPath === '' || documentPath == '/') return;
        var newName = writerApp.correctFileName( titleTextField.text.trim() );
        if (newName.length === 0) {
            updateTitle();
            return;
        } else {
            var entry = writerApp.renameFolder( documentPath, newName );
            if (entry.hasOwnProperty('name')) {
                documentPath = entry.relativePath;
                updateTitle();
                folderNameChanged();
            }
            else {
                //fail
                updateTitle();
                return;
            }
        }
    }

    function updateFolderEmptyIndicator() {
        if (fileModels.size() === 0) {
            documentListView.visible = false;
            folderEmptyContainer.visible = true;
        } else {
            documentListView.visible = true;
            folderEmptyContainer.visible = false;
        }
    }    

//    /**
//     * This handler is invoked when the Document Browser is pushed onto the stack
//     * when the push animation is ended 
//     */ 
//    function handlePushEndedEvent() {
//        updateFolderEmptyIndicator();    
//    }


    /**
     * This handler is invoked periodically by the writerApp instance 
     */
    function handlePeriodicEvent() {
        console.log('[DocumentBrowser]handlePeriodicEvent');
        for (var i = 0; i < fileModels.size(); i++) {
            var entry = fileModels.value(i);
            if (entry.type == 'file') {
                if (entry.hasOwnProperty('nextRefresh')) {
                    // only update if the time is passed
                    var nowTs = (new Date()).getTime();
                    if (nowTs < entry.nextRefresh) 
                        continue;                    
                }
             
                var measure = TimeAssist.measureDistance( entry.modified );
                entry.description = measure.description;
                entry.nextRefresh = measure.nextRefresh;
                fileModels.replace(i,entry);
            }
        }
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
            pageType: 'documentBrowser',
            themeSupport: true
        };
    }
    
    function applyCustomTheme( themeInfo ) {
        if (themeInfo.name == 'Light') {
            themeRowTextColor = themeInfo.textColor;
            rowDividerColor = themeInfo.dividerColor;
            rowHighlightColor = themeInfo.rowHighlightColor;
            moveDescriptionLabel.textStyle.resetColor();
            pageRootContainer.resetBackground();
            titleTextField.textStyle.resetColor();
            folderEmptyLabel.textStyle.resetColor();
            mainDivider.background = Color.create( themeInfo.titleDividerColor );
            return;
        }
        
        if (themeInfo.hasOwnProperty('backgroundColor')) {
            pageRootContainer.background = Color.create( themeInfo.backgroundColor );                
        }
        if (themeInfo.hasOwnProperty('textColor')) {            
            titleTextField.textStyle.color       = Color.create( themeInfo.textColor );
            folderEmptyLabel.textStyle.color     = Color.create(themeInfo.textColor);
            moveDescriptionLabel.textStyle.color = Color.create(themeInfo.textColor);
            // a dilemma
            // listitem's color changed automatically when themeRowTextColor property changed
            // there must be a binding with themeRowTextColor property somewhere ?? #clueless
            themeRowTextColor = themeInfo.textColor;            
        }
        if (themeInfo.hasOwnProperty('dividerColor')) {
            rowDividerColor = themeInfo.dividerColor;            
        } 
        if (themeInfo.hasOwnProperty('titleDividerColor')) {
            mainDivider.background = Color.create( themeInfo.titleDividerColor );
        }
        if (themeInfo.hasOwnProperty('rowHighlightColor')) {
            rowHighlightColor = themeInfo.rowHighlightColor;
        }
    }

    function actionPickTheme() {
        themePickerSheet = themePickerSheetDef.createObject();
        themePickerSheet.closed.connect( onThemePickerSheetClosed );
        themePickerSheet.themePicked.connect( onThemePicked );
        themePickerSheet.open();
    }
    
    function onThemePicked(themeName) {
        themeManager.setTheme(themeName);
    }
    
    function onThemePickerSheetClosed() {
        themePickerSheet.destroy();
    }
    
    ///////////////////////////////////////////////////////////////////////////
    function actionBackupRestore() {
        backupRestoreSheet = backupRestoreSheetDef.createObject();
        backupRestoreSheet.closed.connect(  onBackupRestoreSheetClosed );
        backupRestoreSheet.backup.connect(  onBackupActionSelected );
        backupRestoreSheet.restore.connect( onRestoreActionSelected );
        backupRestoreSheet.open();
    }
    
    function onBackupRestoreSheetClosed() {
        backupRestoreSheet.destroy();
    }

    function onBackupActionSelected() {
        writerApp.actionBackup();
    }
    
    function onRestoreActionSelected() {
        writerApp.actionRestore();
    }
    
    function updateFileMover() {
        if (writerApp.isClipboardEmpty()) {
            fileMoverContainer.visible = false;
        } else {
            fileMoverContainer.visible = true;
            moveButton.enabled = ( documentPath != writerApp.clipboardRelativePath() );
            moveDescriptionLabel.text = writerApp.clipboardDescription();
        }
    }
    
} // end of Page
