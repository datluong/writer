import bb.cascades 1.0

Dialog {
    id: textStyleDialog
    property real fontSizeImmediateValue;
    property bool initialized: false;
    
    Container {
        id: dialogContainer
        opacity: 0
        preferredHeight: 720
        preferredWidth: 720
        layout: DockLayout {}
        background: Color.create(0,0,0,0.15)
        Container {
            // touch-receptive container for handling close 
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            onTouch: {
//                textStyleDialog.close();
                fadeOutAnimation.play();
            }
        }
        Container {            
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Bottom
            background: Color.create(0,0,0,0.5);
            overlapTouchPolicy: OverlapTouchPolicy.Deny
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                topPadding: 12
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Label {
                        textStyle.fontWeight: FontWeight.W200
                        textStyle.color: Color.White
                        text: "Font Size"
                    }
                    Label {
                        id: sizeLabel
                        textStyle.fontWeight: FontWeight.W600
                        textStyle.color: Color.White                        
                    }
                }
            }
            Slider {
                id: fontSizeSlider
                horizontalAlignment: HorizontalAlignment.Center
                fromValue: 5
                toValue: 10
                value: writerApp.editorFontSize();
                onImmediateValueChanged: {
                    var roundedSize = Math.round( immediateValue * 2) / 2.0;
                    if (roundedSize != fontSizeImmediateValue) {
                        fontSizeImmediateValue = roundedSize;
                        sizeLabel.text = '' + roundedSize;
                    }
                }
                
                onValueChanged: {
                    if (initialized) {
                        var roundedSize = Math.round(value * 2) / 2.0;
                        writerApp.setEditorFontSize( roundedSize );
                    }
                }
                
            }
            Container {
                topPadding: 4
            }
        }
        
        animations: [
            FadeTransition {
                id: fadeOutAnimation
                fromOpacity: 1
                toOpacity: 0
                duration: 200
                onEnded: {
                    textStyleDialog.close();
                }
            },
            FadeTransition {
                id: fadeInAnimation
                fromOpacity: 0
                toOpacity: 1
                duration: 200
            }
        ]
    } // end of Main Container
    onOpened: {
        fadeInAnimation.play();
    }
    onCreationCompleted: {
        var fontSize = writerApp.editorFontSize();
        console.log('editorFontSize:', fontSize);
        fontSizeSlider.setValue(fontSize);
        initialized = true;
    }
    function updatePrefSize(width, height) {
        dialogContainer.setPreferredSize(width, height);
    }
} // end of Dialog