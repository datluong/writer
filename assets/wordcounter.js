
function wordCount(text) {
    var regex = /\s+/gi;
    var norm = text.replace(regex, ' ').replace(/^\s+/i, '').replace(/\s+$/i, '');
    if (norm.length === 0) return 0;
    var count =  norm.split(' ').length;
    return count;
}
