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

module.exports.range = range;

/**
 * Return a timestamp range FROM current datetime TO current datetime - hour (specified in range).
 * @params {TimeRange} range - Temporal window
 * @return {from: number, to: number} Time Range
 */
module.exports.computeTimestampRange = function(range){
    let now = new Date();
    return {
        from: addHours(now, -range.hour).getTime(),
        to: now.getTime()
    };
}

/**
 * Compute a statistics value accordly to statistics function used. The value is calculated
 * in a temporal window, aggregating the values using a specific temporal step. 
 * 
 * @callback StatisticFunction
 * @params {Array} orderedItems - Temporally ordered items from the most recent to oldest.
 * @params {string} attribute - The item's attribute to use for compute the statistics
 * @params {{from: number, to: number}} timestampRange - Temporal windows in wich to consider the items.
 * @params {number} stepHour - Temporal step
 * @params {StatisticFunction} statsFunction - Function called in order to compute the value for each item.
 */
function computeStats(orderedItems, attribute, timestampRange, stepHour, statsFunction) {
    let globalStart = new Date(timestampRange.from);
    let globalEnd = new Date(timestampRange.to);
    
    let results = [];
    for(var start = globalStart, end = addHours(globalStart, stepHour);
        end <= globalEnd;
        start = end, end = addHours(start, stepHour))
    {
        results.push(statsFunction(orderedItems, attribute, start, end))
    }
    
    return results;
}


/**
 * Compute a statistic value from all items attribute's, using average as statistic function 
 * @params {Array} orderedItems - Temporally ordered items from the most recent to oldest.
 * @params {string} attribute - The item's attribute to use for compute the statistics
 * @params {{from: number, to: number}} timestampRange - Temporal windows in wich to consider the items.
 * @params {number} stepHour - Temporal step
 */
module.exports.computeAvgStats = function(orderedItems, attribute, timestampRange, stepHour) {
    return computeStats(orderedItems, attribute, timestampRange, stepHour, computeAvgInRange);
}


/**
 * Compute a statistic value from all items attribute's, using sum as statistic function
 * @params {Array} orderedItems - Temporally ordered items from the most recent to oldest.
 * @params {string} attribute - The item's attribute to use for compute the statistics
 * @params {{from: number, to: number}} timestampRange - Temporal windows in wich to consider the items.
 * @params {number} stepHour - Temporal step
 */
module.exports.computeSumStats = function(orderedItems, attribute, timestampRange, stepHour) {
    return computeStats(orderedItems, attribute, timestampRange, stepHour, computeSumInRange);
}


/**
 * Compute sum of all item's attribute
 * @params {Array} items - Temporally ordered items from the most recent to oldest.
 * @params {string} attribute - The item's attribute to use for compute the statistics.
 * @params {number} fromTimestamp - Start of temporal window.
 * @params {number} toTimestamp - End of temporal window.
 */
function computeSumInRange(items, attribute, fromTimestamp, toTimestamp) {
    let sum = items
        .filter(i => i.timestamp >= fromTimestamp && i.timestamp < toTimestamp)
        .map(item => item[attribute])
        .reduce(reduceSum, { sum: 0 });
    
    return {
        timestamp: fromTimestamp.getTime(),
        data: sum.sum
    };
}


/**
 * Compute average of all item's attribute
 * @params {Array} items - Temporally ordered items from the most recent to oldest.
 * @params {string} attribute - The item's attribute to use for compute the statistics.
 * @params {number} fromTimestamp - Start of temporal window.
 * @params {number} toTimestamp - End of temporal window.
 */
function computeAvgInRange(items, attribute, fromTimestamp, toTimestamp) {
    let sumAndCount = items
        .filter(i => i.timestamp >= fromTimestamp && i.timestamp < toTimestamp)
        .map(item => item[attribute])
        .reduce(reduceSumAndCount, { sum: 0, count: 0 });
        
    return {
        timestamp: fromTimestamp.getTime(),
        data: (sumAndCount.count <= 0) ? 0 : sumAndCount.sum / sumAndCount.count
    };
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
 * Add hours to date.
 * @param {Date} currentDate - Date
 * @param {number} hour - Hour to add to date.
 */
function addHours(currentDate, hour) {
    let date = new Date(currentDate.getTime());
    date.setHours(date.getHours() + hour);
    return date;
}