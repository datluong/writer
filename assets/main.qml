import bb.cascades 1.0
import bb.system 1.0
// creates one page with a label
NavigationPane {
    id: rootNavigationPane
    objectName: "rootNavigationPane"
    DocumentBrowser {
        id: rootBrowser
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
        rootBrowser.disableRenaming();
        if (themeManager.currentThemeName() != themeManager.defaultThemeName() ) {
            rootBrowser.applyCustomTheme( themeManager.currentTheme() );
        }
        // disable js logging
//        console.log = function() {}        
    }
    
    function experimentZone() {        
    }
    
    onPushTransitionEnded: {
//        if (page.hasOwnProperty('glMetaData')) {
//            var meta = page.glMetaData();
//            if (meta.pageType == 'documentBrowser') {
//            }
//        }
    }
    onPopTransitionEnded: {
        console.log( '[main]OnPopTransitionEnded', page );
//        if (rootNavigationPane.count() > 0) {
//            var topPage = rootNavigationPane.at( rootNavigationPane.count() - 1);
//            if (topPage.hasOwnProperty('glMetaData')) {
//                var meta = topPage.glMetaData();
//                if (meta.pageType == 'documentBrowser') {
//                    topPage.reloadDirectory();
//                }
//            }
//        }
        
        if ( page.hasOwnProperty('glMetaData') )  {
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
        console.log('[Nav]applyCustomTheme', themeInfo.name );
        for (var i = 0; i < rootNavigationPane.count(); i++) {
            var thePage = rootNavigationPane.at(i);
            if (thePage.hasOwnProperty('glMetaData')) {
                var meta = thePage.glMetaData();
                if (meta.themeSupport === true) 
                    thePage.applyCustomTheme( themeInfo );
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
                if (meta.pageType == 'documentBrowser')
                    thePage.reloadDirectory();
            }
        }
    }
    
} // end of NavigationPane
    