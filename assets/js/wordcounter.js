
//function wc(text) {
    //var regex = /\s+/gi;
    //var norm = text.replace(regex, ' ').replace(/^\s+/i, '').replace(/\s+$/i, '');
    //if (norm.length === 0) return 0;
    //var count =  norm.split(' ').length;
    //return count;
//}

/**
 * High performance wordCount function with O(n) speed
 */
function wordCount(text) {
    var wordStartSeekMode = true;
    var count = 0;
    var cursor = 0;

    while (cursor <= text.length) {
        var ch = (cursor == text.length ? " " : text[cursor] );
        cursor ++;

        if (wordStartSeekMode) {
            // in this mode, we seek the position of the start character in a word
            //if (whitespaceRegex.test(ch)) continue; // next char
            if ( ch == " " || ch == "\b" || ch == "\t" || ch == "\n" || ch == "\v" || ch == "\f" || ch == "\r" ) continue;
            // meet a non-whitespace char
            wordStartSeekMode = false;
            continue;
        }
        else {
            // in this mode, seek the last character in a word
            var isWhiteSpace =  ( ch == " " || ch == "\b" || ch == "\t" || ch == "\n" || ch == "\v" || ch == "\f" || ch == "\r" );
            if (isWhiteSpace === false) continue;

            // white space encountered
            count++;
            wordStartSeekMode = true;
        }
    }

    return count;
}

function wcOrigin(text) {
    //var whitespaceRegex = /^\s$/;
    //var punctuationsRegex = /^[\^\-\[\]\\\/!@#$%&*()+_={}:;"'|<>?.,]+$/;

    var wordStartSeekMode = true;
    var wordStart = -1;
    var wordEnd = -1; // word "abc " -> wordEnd point to " "
    var count = 0;
    var cursor = 0;

    while (cursor <= text.length) {
        var ch = (cursor == text.length ? " " : text[cursor] );
        cursor ++;

        if (wordStartSeekMode) {
            // in this mode, we seek the position of the start character in a word
            //if (whitespaceRegex.test(ch)) continue; // next char
            if ( ch == " " || ch == "\b" || ch == "\t" || ch == "\n" || ch == "\v" || ch == "\f" || ch == "\r" ) continue;
            // meet a non-whitespace char
            //wordStart = cursor - 1;
            wordStartSeekMode = false;
            continue;
        }
        else {
            // in this mode, seek the last character in a word
            //if (whitespaceRegex.test(ch) === false) continue;
            var isWhiteSpace =  ( ch == " " || ch == "\b" || ch == "\t" || ch == "\n" || ch == "\v" || ch == "\f" || ch == "\r" );
            if (isWhiteSpace === false) continue;

            // white space encountered
            //wordEnd = cursor - 1;
            // test the word
            var word = text.substring(wordStart,wordEnd);
            if (!punctuationsRegex.test(word)) {
                //console.log('got Word:', word);
                count++;
            }

            wordStartSeekMode = true;
        }
    }

    return count;
}

//function performanceTest( text ) {
    //var loopCount = 10;
    //var st1 = (new Date()).getTime();
    //var i=0;
    //for ( i = 0; i < loopCount; i++) {
        //wordCount(text);
    //}
    //var st2 = (new Date()).getTime();
    //for ( i = 0; i < loopCount; i++) {
        //wc(text);
    //}    
    //var st3 = (new Date()).getTime();

    //console.log('wordCount():', st2-st1, 'ms');
    //console.log('wc():', st3-st2, 'ms');
//}

// punctuations
// no escape   : !@#$%&*()+_={}:;"'|<>?.,
// must escape : ^-[]\/
//
// var punctuationsRegex = /^[\^\-\[\]\\\/!@#$%&*()+_={}:;"'|<>?.,]+$/

