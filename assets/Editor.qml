import bb.cascades 1.0
import my.timer 1.0

Page {
    id: editorPage
    objectName: "editorPage"
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.Disabled
    actionBarVisibility: ChromeVisibility.Hidden
    
    property string documentTitle: '';
    property string documentPath: '';
    property bool   readyForEditing: false;
    property bool   documentChanged: false;
    property bool   compactWordCountView: false;
    property bool   wordCountBarHidden: false;
    property bool   timerActive: false;
    property double lastStrokeTime: 0;
    property double wordCountBarIdleTime: 2500
    property variant themePickerSheet;
    
    signal documentTitleUpdated(string newTitle);
    signal documentUpdated(string path);
    
    Container {
        id: rootContainer
        horizontalAlignment: HorizontalAlignment.Fill
        layout: DockLayout {}
        ScrollView {                
            Container {                     
                id: editorContainer
                layout: StackLayout {}
                Container {
                    id: editorTitleContainer
                    leftPadding: 24
                    rightPadding: 24
                    topPadding: 36
                    bottomPadding: 24
                    TextArea {
                        id: titleTextArea
                        hintText: "Title"
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
                            if (text.length > 0 && text.indexOf("\n") == text.length-1) {
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
                        textStyle.fontWeight: FontWeight.W400
                        textStyle.fontSize: FontSize.PointValue
                        // fontSize 7 is small but readable. Recommended size is 7.5  
                        textStyle.fontSizeValue: 7
                        scrollMode: TextAreaScrollMode.Elastic
                        hintText: 'Content'
                        minHeight: 480
                        text: ""
                        implicitLayoutAnimationsEnabled: false
                        onTextChanging: {          
                            documentChanged = true;
                            updateWordCount(wordCount(text));
                            
                            if (compactWordCountView && readyForEditing) {
                                hideWordCountBar();
                            }
                        }
                        onFocusedChanged: {
                            if (focused)
                                textFieldGotFocus();
                        }                        
                    }
                }
                Container {
                    // padding Container
                    horizontalAlignment: HorizontalAlignment.Fill
                    background: Color.Transparent
                    preferredHeight: 44
                }
            } // end Editor Container
        } // end ScrollView
        Container {
            id: wordCountBar
            
            preferredHeight: 52
            rightPadding: 8
            
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Bottom
            Container {
                id: controllerInnerContainer
                preferredHeight: 44                
                background: Color.create(0, 0, 0, 0.6)
                leftPadding: 12
                rightPadding: 14
                Container {
                    preferredHeight: 44
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    animations: [
                        FadeTransition {
                            id: touchAnimation                            
                            duration: 100
                            fromOpacity: 0.2
                            toOpacity: 1
                            onEnded: {
                                if (editorPage.actionBarVisibility == ChromeVisibility.Hidden) {
                                    editorPage.actionBarVisibility = ChromeVisibility.Visible;
                                } else {
                                    editorPage.actionBarVisibility = ChromeVisibility.Hidden;
                                }                                    
                            }                            
                        }
                    ]
                    Label {
                        id: wordCountLabel
                        text: "0 word"
                        textStyle.fontWeight: FontWeight.W200
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 7
                        textStyle.color: Color.White                      
                    }
                    ImageView {
                        id: wordCountIcon                    
                        imageSource: "asset:///images/dots.png"
                        verticalAlignment: VerticalAlignment.Center
                    }                    
                }
                
            }
            onTouch: {
                if (event.isUp()) {
                    touchAnimation.play();                         
                }
            }
        } // end Controller Container
        
        attachedObjects: [
            LayoutUpdateHandler {
                onLayoutFrameChanged: {
                    console.log('[editor-RootContainer]onLayoutFrameChanged', layoutFrame.width, layoutFrame.height);
                    textStyleDialog.updatePrefSize( layoutFrame.width, layoutFrame.height );
                    if (!compactWordCountView) {
                        var isVirtualKbShown = writerApp.determineVirtualKeyboardShown(layoutFrame.width, layoutFrame.height);
                        wordCountBar.opacity = isVirtualKbShown ? 0:1;
                        wordCountBarHidden   = isVirtualKbShown;
                    }
                }
            }
        ]
    } // end Root Container
    
    attachedObjects: [
        QTimer {
            id: editorTimer
            singleShot: false
            interval: 1000            
            onTimeout: {
                updateWordCountBarVisibility();
            }
        },
        ActionItem {
            id: saveActionItem
            title: "Save"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/icon-save.png"
            onTriggered: {
                saveDocument();
            }
        },
        ActionItem {
            id: exitActionItem
            title: "Exit"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/icon-exit.png"
            onTriggered: {
                Application.requestExit();
            }
        },
        TextStyleDialog {
            id: textStyleDialog
            onFontSizeImmediateValueChanged: {
                var roundedSize = textStyleDialog.fontSizeImmediateValue;
                if (editorTextArea.textStyle.fontSizeValue != roundedSize) {
                    console.log('update -> ', roundedSize);
                    editorTextArea.textStyle.fontSizeValue = roundedSize;
                }
            }
        }
    ]
    
    actions: [
        ActionItem {
            title: "Share"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/ic_share.png"
            onTriggered: {
                writerApp.actionShareDocument( titleTextArea.text, editorTextArea.text );
            }
        },
        ActionItem {
            title: "Share As File"
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/ic_share_file.png"
            onTriggered: {
                writerApp.actionShareDocumentAsAttachment(titleTextArea.text, editorTextArea.text);
            }
        },
        ActionItem {
            title: "Save As"
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/ic_save_as.png"
            onTriggered: {
                writerApp.actionSaveToSharedFolder(titleTextArea.text, editorTextArea.text);
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
            title: "Font Size"
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/icon-font-size.png"
            onTriggered: {
                textStyleDialog.open();
            }
        }
    ]
    
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
        compactWordCountView = writerApp.isPhysicalKeyboardDevice();
        if (compactWordCountView) {
            editorTimer.start();
            timerActive = true;
        }
        
        updateWordCount( wordCount(editorTextArea.text) );                
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Q10/Q5 WordCountBar visibility handling
     function onFullscreen() {
        if (compactWordCountView && timerActive === false) {
            timerActive = true;
            editorTimer.start();
            console.log('editorTimer is started');
        }        
    }

    function onThumbnailed() {
        if (compactWordCountView && timerActive) {
            timerActive = false;
            editorTimer.stop();
            console.log('editorTimer is stopped');
        }
    }
    
    function hideWordCountBar() {
        var now = new Date();
        lastStrokeTime = now.getTime();
        if (wordCountBarHidden) return;
        
        wordCountBarHidden = true;
        wordCountBar.opacity = 0;         
    }
    
    function updateWordCountBarVisibility() {
        if (wordCountBarHidden === false) return;
        
        var now = (new Date()).getTime();
        if (now > lastStrokeTime + wordCountBarIdleTime) {
            // show wordCountBar
            wordCountBarHidden = false;
            wordCountBar.opacity = 1;
        }
        else if (now < lastStrokeTime) {
            lastStrokeTime = now;
        }
            
    } 
    ///////////////////////////////////////////////////////////////////////////
    
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
        return { pageType: 'editor',
                 themeSupport: true
        };
    }
    
    function beginEditing() {
        documentChanged = false;
        readyForEditing = true;
    }
    /**
     * Save document
     */
    function saveDocument() {      
        if (!documentChanged) {
            // save if title don't match path
            if (writerApp.documentPathMathTitle(documentPath, titleTextArea.text))
                return;
        }
        var status = writerApp.saveDocument( documentPath, titleTextArea.text, editorTextArea.text, {});
        console.log('saveDocument. result: ', status, 'documentchanged:', documentChanged );
        if (status == 1) { // document title has changed
            documentTitleUpdated(titleTextArea.text);            
        }
                
        if (status != 0 && status != 1) {
            // TODO show error message
        }
        else {                             
            documentUpdated( documentPath );            
        }
    }
    
    function handleAppExitEvent() {
        saveDocument();
    }
    
    function handleAutoSaveEvent() {
        if (documentChanged) {
            console.log('autosaving', documentPath);
            var status = writerApp.saveDocument(documentPath, titleTextArea.text, editorTextArea.text, {
                    keepTitle:true
                });
            console.log('[autoSave]saveDocument. result: ', status);
            documentChanged = false;
            if (status != 0 && status != -1) {
                // TODO show error message
            }
        }
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

    function applyCustomTheme(themeInfo) {
        if (themeInfo.name == 'Light') { //default theme
            rootContainer.resetBackground();
            editorContainer.resetBackground();
            titleTextArea.textStyle.resetColor();
            editorTextArea.textStyle.resetColor();
            controllerInnerContainer.background = Color.create(0, 0, 0, 0.6);
            wordCountLabel.textStyle.color = Color.White;
            wordCountIcon.imageSource = "asset:///images/dots.png";
            return;
        }
        
        if (themeInfo.hasOwnProperty('backgroundColor')) {
            rootContainer.background = Color.create( themeInfo.backgroundColor );
            editorContainer.background = Color.create(themeInfo.backgroundColor);
        }
        
        if (themeInfo.hasOwnProperty('textColor')) {
            titleTextArea.textStyle.color = Color.create( themeInfo.textColor );
            editorTextArea.textStyle.color = Color.create( themeInfo.textColor );
        }
        
//        if (themeInfo.hasOwnProperty('')) {}
        if (themeInfo.hasOwnProperty('wordCountBackgroundColor')) {
            controllerInnerContainer.background = Color.create( themeInfo.wordCountBackgroundColor );
        }
               
        if (themeInfo.hasOwnProperty('wordCountTextColor')) {
            wordCountLabel.textStyle.color = Color.create( themeInfo.wordCountTextColor);
        }
        
        if (themeInfo.hasOwnProperty('wordCountDotsImageSource')) {
            wordCountIcon.imageSource = themeInfo.wordCountDotsImageSource;
        }
    }

    function actionPickTheme() {
        themePickerSheet = themePickerSheetDef.createObject();
        themePickerSheet.closed.connect(onThemePickerSheetClosed);
        themePickerSheet.themePicked.connect(onThemePicked);
        themePickerSheet.open();
    }

    function onThemePicked(themeName) {
        themeManager.setTheme(themeName);
    }

    function onThemePickerSheetClosed() {
        themePickerSheet.destroy();
    }

    function enableSaveButton() {
        editorPage.addAction( saveActionItem );
        editorPage.addAction( exitActionItem );
    }
} // end Page
