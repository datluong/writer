
/**
 *  return: { description: {string}, nextRefresh: {number} }
 */
function measureDistance( msSinceEpoch ) {
    var d = new Date( msSinceEpoch );
    var now = new Date();

    // if the distance is year
    var yearDistance = now.getFullYear() - d.getFullYear();
    if (yearDistance > 0) {
        return { description: ( '' + yearDistance + ' ' + (yearDistance>1?'years':'year') ),
                 nextRefresh: ( new Date(now.getFullYear()+1,0,1) ).getTime() };
    }
    else if (yearDistance < 0) {
        return { description: toShortDate(d),
                 nextRefresh: d.getTime() };

    }

    // same year
    var monthDistance = now.getMonth() - d.getMonth();
    if (monthDistance > 0) {
        return { description: ( '' + monthDistance + ' ' + (monthDistance>1?'months':'month')),
                 nextRefresh: ( new Date(now.getFullYear(), now.getMonth()+1, 1 ) ).getTime() };
    } else if (monthDistance < 0) {
        return { description: toShortDate(d),
                 nextRefresh: d.getTime() };
    }
    
    // same month & year
    var dayDistance = now.getDate() - d.getDate();
    if (dayDistance > 0) {
        return { description: ( '' + dayDistance + ' ' + (dayDistance>1?'days':'day')),
                 nextRefresh: ( new Date(now.getFullYear(), now.getMonth(), now.getDate()+1 ) ).getTime() };        
    } else if (dayDistance < 0) {
        return { description: toShortDate(d),
                 nextRefresh: d.getTime() };        
    }

    // same day
    var hourDistance =  now.getHours() - d.getHours();
    if (hourDistance > 0) {
        return { description: ( '' + hourDistance + ' ' + (hourDistance>1?'hours':'hour')),
                 nextRefresh: ( new Date(now.getFullYear(), now.getMonth(), now.getDate(), now.getHours()+1,0,0) ).getTime() }; 
    }
    else if (hourDistance < 0) {
        return { description: toShortTime(d),
                 nextRefresh: d.getTime() };
    }

    // same hours
    var minuteDistance = now.getMinutes() - d.getMinutes();
    if (minuteDistance > 0) {
        return { description: ( '' + minuteDistance + ' ' + (minuteDistance>1?'minutes':'minute')),
                 nextRefresh: ( new Date(now.getFullYear(), now.getMonth(), now.getDate(), now.getHours(),now.getMinutes()+1,0) ).getTime() };
    }
    else if (minuteDistance < 0) {
        return { description: toShortTime(d),
                 nextRefresh: d.getTime() };        
    }

    // same minutes
    var secondDistance = now.getSeconds() - d.getSeconds();
    if (secondDistance > 0) {
        return { description: ( '' + secondDistance + ' ' + (secondDistance>1?'seconds':'second')),
                 nextRefresh: ( new Date(now.getFullYear(), now.getMonth(), now.getDate(), now.getHours(),now.getMinutes(), now.getSeconds()+1) ).getTime() };  
    }
    else if (secondDistance < 0) {
        return { description: toShortTime(d),
                 nextRefresh: d.getTime() };                
    }

    // same seconds    
    return { description: 'just now',
             nextRefresh:  ( new Date(now.getFullYear(), now.getMonth(), now.getDate(), now.getHours(),now.getMinutes(), now.getSeconds()+1) ).getTime() };
}

/**
 * Get the date in MM/dd/yyyy format
 **/
function toShortDate( theDate ) {
    return '' + (theDate.getMonth()+1) + '/' + theDate.getDate() + '/' + theDate.getFullYear();
}

function toShortTime( theDate ) {
    var monthStr = '' + theDate.getMinutes();
    if (monthStr.length == 1) monthStr = '0' + monthStr;

    return theDate.getHours() + ':' + monthStr;
}
