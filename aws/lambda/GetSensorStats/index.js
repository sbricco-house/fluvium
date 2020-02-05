const aws = require("./aws-config")
const http = require("./http-responses")
const validator = require("./validator")
const stats = require("./statistics")

/* Keys of attributes*/
const KEY_DELTA = "delta";
const KEY_HUMIDITY = "soil_moisture";  
const KEY_RAIN = "rain_quantity";
const KEY_TIMERANGE = "time_range";

/**
 * Payload required:
 * - deviceName => "nameofdeviece"
 * - attribute  => "delta" | "soil_moisture" | "rain_quantity"
 * - timeRange  => "day" | "week" | "month"
 * 
 * Example:
 * {
 *      deviceName: "waterlevel:cesena:1",
 *      attribute: "soil_moisture",
 *      timeRange: "day"
 * }
 */
exports.handler = async (event) => {
    if(!validator.validateMandatoryArgs(event, 'attribute', 'deviceName')) {
        return http.response400('Missing parameters');
    }
    
    if(!validateAttribute(event.attribute)) {
        return http.response400('Invalid key type, allowed are: ...');       
    }
    
    let timeRange = parseTimeRange(event.timeRange);
    if(!timeRange) {
        return http.response400("Invalid timerange");
    }
    
    // compute a temporal window accorldy to user specified time range.
    let timestampRange = stats.computeTimestampRange(timeRange);
    
    let queryParams =  {
        ExpressionAttributeValues: {
            ':device' : event.deviceName,
            ':from' : timestampRange.from,
            ':to' : timestampRange.to
        },
        ExpressionAttributeNames: {
            '#ts': "Timestamp"
        },
        KeyConditionExpression: 'DeviceName = :device AND #ts BETWEEN :from AND :to',
        TableName: getTableName(event.attribute),
        ScanIndexForward: true
    }
    
    // only Rain quantity need to compute a sum statistic, other attributes use the average.
    let statsFunction = event.attribute == KEY_RAIN ? stats.computeSumStats : stats.computeAvgStats;
    
    // execute query; format the items and compute the stats.
    return await aws.dbClient.query(queryParams).promise()
        .then(data => data.Items.map(item => serializeItem(item, event.attribute)))
        .then(items => http.response200(statsFunction(items, event.attribute, timestampRange, timeRange.stepHour)))
        .catch(err => http.response500(err.message));
};

/**
 * Create a new JSON, unwrapping attribute from item and adding a timestamp.
 */
function serializeItem(item, dataAttribute) {
    let obj = {};
    obj['timestamp'] = item.Timestamp;
    obj[dataAttribute] = item.Data[dataAttribute];
    return obj;
}

function validateAttribute(attribute) {
    return attribute == KEY_DELTA || attribute == KEY_HUMIDITY || attribute == KEY_RAIN;
};

function parseTimeRange(timerange) {
    switch (timerange) {
        case 'day': return stats.range.DAY;
        case 'week': return stats.range.WEEK;
        case 'month' : return stats.range.MONTH;
        default: return false;
    }
}

function getTableName(attribute) {
    return attribute == KEY_DELTA ? "WaterLevelHistory" : "GroundDataHistory";
}