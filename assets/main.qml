import bb.cascades 1.0
import bb.system 1.0
// creates one page with a label
NavigationPane {
    id: rootNavigationPane
    objectName: "rootNavigationPane"
    DocumentBrowser {
        
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
        SystemDialog {
            id: deleteConfirmationDialog
            title: "Delete"
            body: "Delete the selected document?"
        }
    ]
    
    onCreationCompleted: {
        // disable js logging
//        console.log = function() {}        
    }
    
    function experimentZone() {        
    }
    
    onPushTransitionEnded: {
        if (page.hasOwnProperty('glMetaData')) {
            var meta = page.glMetaData();
            if (meta.pageType == 'documentBrowser') {
                console.log('browserPushed.');
            }
        }
    }
    onPopTransitionEnded: {
        console.log( '[main]OnPopTransitionEnded', page );        
        if ( page.hasOwnProperty('glMetaData') )  {
            var meta = page.glMetaData();
            if (meta.pageType == 'editor') {
                page.saveDocument();
            }
        }
        page.destroy();
    }
} // end of NavigationPane
    