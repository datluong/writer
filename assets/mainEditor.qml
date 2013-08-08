import bb.cascades 1.0
import bb.system 1.0
// creates one page with a label
NavigationPane {
    id: rootNavigationPane
    objectName: "rootNavigationPane"
    Editor {
        id: rootEditor
    }
    attachedObjects: [
        ComponentDefinition {
            id: editorPageDef
            source: "Editor.qml"
        },
        ComponentDefinition {
            id: documentBrowserPageDef
            source: "DocumentBrowser.qml"
        },
        ComponentDefinition {
            id: themePickerSheetDef
            source: "ThemePickerSheet.qml"
        },
        ComponentDefinition {
            id: backupRestoreSheetDef
            source: "BackupRestoreSheet.qml"
        },
        SystemDialog {
            id: deleteConfirmationDialog
            title: "Delete"
            body: "Delete the selected document?"
        },
        SystemToast {
            id: mainMessageToast
            body: ""
        }
    ]

    onCreationCompleted: {
        if (themeManager.currentThemeName() != themeManager.defaultThemeName()) {
//            rootBrowser.applyCustomTheme(themeManager.currentTheme());
            rootEditor.applyCustomTheme(themeManager.currentTheme());
        }
        
        // enable save button 
        rootEditor.enableSaveButton();
        
        // disable js logging
        //        console.log = function() {}
    }
    
    /**
     * When mainEditor is loaded by an invoke request, after the
     * editor is created, we load it with the data of the invokeRequest
     */
    function onRequestInvoked() {
        var requestInfo = writerApp.invokeOptions();
        if (!requestInfo.hasOwnProperty('uri')) 
            return; //TODO handle data option
        var filePath = requestInfo.uri;
        if (filePath.indexOf("file://") === 0)
            filePath = filePath.replace("file://","");
        console.log('[mainEditor]onRequestInvoked:uri', requestInfo.uri, ":path:", filePath);
        
        if (!writerApp.isFileLoadable(filePath)) {
            //TODO show error message
            return;
        }

        var text = writerApp.loadFileContent(filePath);
        if (text == '' || text.length == 0 || typeof (text) != 'string') text = '';
        
        var fileInfo = writerApp.getFileInfo(filePath);

        rootEditor.documentTitle = fileInfo.name;
        rootEditor.documentPath = fileInfo.path;
        rootEditor.setEditorBodyContent(text);        
        rootEditor.beginEditing();

        rootEditorr.focusEditor();        
    }

    function experimentZone() {
    }

    onPopTransitionEnded: {
        console.log('[main]OnPopTransitionEnded', page);
        //        if (rootNavigationPane.count() > 0) {
        //            var topPage = rootNavigationPane.at( rootNavigationPane.count() - 1);
        //            if (topPage.hasOwnProperty('glMetaData')) {
        //                var meta = topPage.glMetaData();
        //                if (meta.pageType == 'documentBrowser') {
        //                    topPage.reloadDirectory();
        //                }
        //            }
        //        }

        if (page.hasOwnProperty('glMetaData')) {
            var meta = page.glMetaData();
            if (meta.pageType == 'editor') {
                page.saveDocument();
                writerApp.unRegisterDocumentInEditing();
            }
        }
        page.destroy();
    }

    function applyCustomTheme() {
        var themeInfo = themeManager.currentTheme()
        console.log('[Nav]applyCustomTheme', themeInfo.name);
        for (var i = 0; i < rootNavigationPane.count(); i ++) {
            var thePage = rootNavigationPane.at(i);
            if (thePage.hasOwnProperty('glMetaData')) {
                var meta = thePage.glMetaData();
                if (meta.themeSupport === true) thePage.applyCustomTheme(themeInfo);
            }
        }
    }

    /**
     * Pop all Pages on stack to root page and refresh the root page
     */
    function resetNavigationStack() {
        while (rootNavigationPane.count() > 1) {
            rootNavigationPane.pop();
        }
        if (rootNavigationPane.count() == 1) {
            var thePage = rootNavigationPane.at(0);
            if (thePage.hasOwnProperty('glMetaData')) {
                var meta = thePage.glMetaData();
                if (meta.pageType == 'documentBrowser') thePage.reloadDirectory();
            }
        }
    }
    
    /**
     * Do nothing
     */
    function onClipboardChanged() {}
    function onFilesMoved() {}

}// end of NavigationPane
