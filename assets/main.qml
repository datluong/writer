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
        SystemDialog {
            id: deleteConfirmationDialog
            title: "Delete"
            body: "Delete the selected document?"
        }
    ]
    
    function experimentZone() {        
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
    