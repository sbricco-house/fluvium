/**
 * Time range for each allowed temporal window.
 * @typedef{{hour: number, stepHour: number}} TimeRange
 * @readonly
 * @enum {TimeRange}
 */
const range = {
    DAY: { hour: 24, stepHour: 1 },
    WEEK: { hour: 7 * 24, stepHour: 24 },
    MONTH: { hour: 30 *  24, stepHour: 24 }
}

/**
 * Reduce function that sum and count the elements
 * @param {number} acc - Accumulator
 * @param {number} elem - Element to add and count.
 */
function reduceSumAndCount(acc, elem) {
    return {
        sum: acc.sum + elem,
        count: acc.count + 1
    };
}

/**
 * Reduce function that accumulate and sum the elements.
 * @param {number} acc - Accumulator.
 * @param {number} elem - Element to add.
 */
function reduceSum(acc, elem) {
    return { sum: acc.sum + elem };
}

/**
 * Add minutes to date.
 * @param {Date} currentDate - Date
 * @param {number} minutes - minute to add to date.
 */
function addMinutes(currentDate, minutes) {
    let date = new Date(currentDate.getTime());
    date.setMinutes(date.getMinutes() + minutes);
    return date;
}


// EXPORTS
module.exports.range = range;

/**
 * Return a timestamp range FROM current datetime TO current datetime - hour (specified in range).
 * @params {number} pastMinute - Minute to subtract from now
 * @return {from: number, to: number} Temporal window
 */
module.exports.getTimestampIntervalFromNow = function(pastMinute) {
    let now = new Date();
    //let now = new Date(1580152424568); // used for test
    return {
        from: addMinutes(now, -pastMinute).getTime(),
        to: now.getTime()
    };
}

/**
 * Compute a statistic value from all items attribute's, using average as statistic function 
 * @params {Array} orderedItems - Temporally ordered items from the most recent to oldest.
 */
module.exports.computeAvgStats = function(orderedItems) {
    let sumAndCount = orderedItems.reduce(reduceSumAndCount, { sum: 0, count: 0 });
    return (sumAndCount.count <= 0) ? 0 : sumAndCount.sum / sumAndCount.count;
}

/**
 * Compute a statistic value from all items attribute's, using sum as statistic function
 * @params {Array} orderedItems - Temporally ordered items from the most recent to oldest.
 */
module.exports.computeSumStats = function(orderedItems) {
    return orderedItems.reduce(reduceSum, { sum: 0 }).sum;   
}
