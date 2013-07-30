import bb.cascades 1.0

Page {
    id: editorPage
    objectName: "editorPage"
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.Disabled
    actionBarVisibility: ChromeVisibility.Hidden
    property string documentTitle: '';
    property string documentPath: '';
    property bool   documentChanged: false;
    signal documentTitleUpdated(string newTitle);
    
    Container {
        id: rootContainer
        layout: DockLayout {}
        ScrollView {                
            Container {
                id: editorContainer
                layout: StackLayout {}
                Container {
                    id: editorTitleContainer
//                    background: Color.Red
                    leftPadding: 24
                    rightPadding: 24
                    topPadding: 36
                    bottomPadding: 24
                    TextArea {
                        id: titleTextArea
                        hintText: "Untitled"
                        topPadding: 0
                        bottomPadding: 0
                        text: ""
                        backgroundVisible: false
                        textStyle.fontWeight: FontWeight.W500
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 8.5
                        onFocusedChanged: {
                            if (focused) 
                                textFieldGotFocus();
                        }
                        onTextChanging: {
                            documentChanged = true;
                            console.log('[titleTextArea]onTextChanging ->',text);
                            if (text.length > 0 && text.indexOf("\n") == text.length-1) {
                                console.log('enter key detected');
                                // remove the enterKey
                                titleTextArea.text = text.substring(0, text.length-1) ;                               
                                focusEditor();
                            }
                        }
                    }
                }
                Container {}
                Container {
                    id: editorBodyContainer
                    topPadding: 0
                    leftPadding: 24
                    rightPadding: 24
                    TextArea {
                        
                        id: editorTextArea
                        topPadding: 0
                        backgroundVisible: false
                        focusHighlightEnabled: false
                        textFormat: TextFormat.Plain
                        textStyle.fontWeight: FontWeight.W200
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 7
                        scrollMode: TextAreaScrollMode.Elastic
                        hintText: ''
                        minHeight: 480
                        text: ""
                        implicitLayoutAnimationsEnabled: false
                        onTextChanging: {          
                            documentChanged = true;
                            updateWordCount(wordCount(text));
                        }
                        onFocusedChanged: {
                            if (focused)
                                textFieldGotFocus();
                        }
                        
                    }
                }
            } // end Editor Container
        } // end ScrollView
        Container {
            id: controllerContainer
            
            preferredHeight: 52
            rightPadding: 30
            
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Bottom
            Container {
                id: controllerInnerContainer
                preferredHeight: 44                
                background: Color.create(0, 0, 0, 0.6)
                leftPadding: 12
                rightPadding: 14
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Label {
                    id: wordCountLabel
                    text: "0 word"
                    textStyle.fontWeight: FontWeight.W200
                    textStyle.fontSize: FontSize.PointValue
                    textStyle.fontSizeValue: 7
                    textStyle.color: Color.White
                }
                ImageView {
                    imageSource: "asset:///images/dots.png"
                    verticalAlignment: VerticalAlignment.Center
                }
            }
            onTouch: {
                if (event.isUp()) {
                    if (editorPage.actionBarVisibility == ChromeVisibility.Hidden) {
                        editorPage.actionBarVisibility = ChromeVisibility.Visible;
                    }
                    else {
                        editorPage.actionBarVisibility = ChromeVisibility.Hidden;
                    }                    
                }
            }
        } // end Controller Container
    } // end Root Container 
    
    function updateWordCount(count) {
        var t = '' + count + (count < 2 ? ' word' : ' words');
        wordCountLabel.setText( t );        
    }
    
    function wordCount(text) {
        var regex = /\s+/gi;
        var norm = text.replace(regex, ' ').replace(/^\s+/i, '').replace(/\s+$/i, '');
        if (norm.length === 0) return 0;
        var count =  norm.split(' ').length;
        return count;
    }
    
    onCreationCompleted: {
        updateWordCount( wordCount(editorTextArea.text) );
    }
    
    function textFieldGotFocus() {
        if (editorPage.actionBarVisibility != ChromeVisibility.Hidden)
            editorPage.actionBarVisibility = ChromeVisibility.Hidden;
    }
    
    // update editor's content
    function setEditorBodyContent(content) {        
        console.log('setEditorBodyContent', content.length, 'type', typeof (content));
        if (content.length > 0) {
            editorTextArea.text = content;
        } else {
            editorTextArea.resetText();
        }
    }
    
    // event handlers of Editor Page
    onDocumentTitleChanged: {
        titleTextArea.text = documentTitle;
    }
    
    /**
     * QML Page defines glMetaData() method for inter-operating with other QML Objects  
     */
    function glMetaData() {
        return { pageType: 'editor' };
    }
    
    function beginEditing() {
        documentChanged = false;
    }
    /**
     * Save document
     */
    function saveDocument() {      
        if (!documentChanged) return;
        var status = writerApp.saveDocument( documentPath, titleTextArea.text, editorTextArea.text);
        console.log('saveDocument. result: ', status, 'documentchanged:', documentChanged );
        if (status == 1) { // document title has changed
            documentTitleUpdated(titleTextArea.text);            
        }
        if (status != 0 && status != 1) {
            // TODO show error message
        }
    }
    
    function handleAppExitEvent() {
        saveDocument();
    }
    
    function focusEditor() {
        editorTextArea.requestFocus();
    }
    
    function focusTitle() {
        titleTextArea.requestFocus();
    }
    
    function clearTitle() {
        titleTextArea.resetText();
    }
} // end Page
