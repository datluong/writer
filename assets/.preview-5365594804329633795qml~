import bb.cascades 1.0

// creates one page with a label
NavigationPane {
    id: rootNavigationPane
    DocumentBrowser {
        
    }   
    attachedObjects: [
        ComponentDefinition {
            id: editorPageDef
            source: "Editor.qml"
        }
    ]
    
    function experimentZone() {
//        rootNavigationPane.pus
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

