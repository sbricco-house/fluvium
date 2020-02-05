const aws = require("./aws-config")
const stats = require("./statistics")

const PERIODIC_TIME_MINUTE = 30;
const KEY_DELTA = "delta";
const KEY_RAIN_QUANTITY = "rain_quantity";

/**
 * No argument are required. This function is called from a periodic cloud watch function.
 * For each River, it update the average delta water level and rain quantity.
 **/
exports.handler = async (event) => {
    // get a timestamp interval from now to now - PERIODIC_TIME_MINUTE
    let timeRange = stats.getTimestampIntervalFromNow(PERIODIC_TIME_MINUTE);
    // retrive all rivers from AWS IoT
    let rivers = await getAllRivers();
    
    
    // calculate and update each river
    for(let riverName of rivers) {
        let devices = await aws.iot.listThingsInThingGroup({thingGroupName: riverName}).promise();
        
        // calculate the delta values for each device
        let devicesDeltaValues = await devices.things.mapAsync(deviceName => computeStatsOfDevice(deviceName, KEY_DELTA, stats.computeAvgStats, timeRange));
        // calculate the rain quantity values for each device
        let devicesRainValues = await devices.things.mapAsync(deviceName => computeStatsOfDevice(deviceName, KEY_RAIN_QUANTITY, stats.computeSumStats, timeRange));
        
        // compute average for delta and rain quantity
        let avgDelta = stats.computeAvgStats(devicesDeltaValues);
        let avgRainQuantity = stats.computeAvgStats(devicesRainValues);
        
        // update the device's group attributes.
        let groupParams = {
            thingGroupName: riverName,
            thingGroupProperties: {
                attributePayload: {
                    attributes: {
                        'lastUpdate': timeRange.to, 
                        'deltaLevelAvg': avgDelta.toString(),
                        'rainQuantityAvg': avgRainQuantity.toString()
                    },
                  merge: true
                }
            }
        };
        
        await aws.iot.updateThingGroup(groupParams).promise()
            .then(data => {}/* do nothing */ )
            .catch(err => console.log(err) );
    }
};

/**
 * Return all rivers from AWS IoT
 */
async function getAllRivers() {
    let params = { recursive: false };
    return await aws.iot.listThingGroups(params).promise()
        .then(data => data.thingGroups.map(item => item.groupName))
        .catch(err => { console.log(err); return []; });
}

/**
 * Compute statistic of single device.
 * @callback FunctionOverItems
 * @param {string} deviceName - Name of device
 * @param {string} attribute - Attribute to use for compute the statistic.
 * @param {FunctionOverItems} statsFunctionOverItems - Statistic function to apply of device's items
 * @param {{from: number, to: number}} timeRange - Temporal window.
 */
async function computeStatsOfDevice(deviceName, attribute, statsFunctionOverItems, timeRange) {
    let queryParams =  {
        ExpressionAttributeValues: {
            ':device' : deviceName,
            ':from' : timeRange.from,
            ':to' : timeRange.to
        },
        ExpressionAttributeNames: {
            '#ts': "Timestamp"
        },
        KeyConditionExpression: 'DeviceName = :device and #ts BETWEEN :from AND :to',
        TableName: getTableName(attribute),
        ScanIndexForward: true
    };
    
    return await aws.dbClient.query(queryParams).promise()
        .then(data => data.Items.map(item => item.Data[attribute]))
        .then(items => statsFunctionOverItems(items));
}

Array.prototype.mapAsync = async function(mapFunction){
    return Promise.all(this.map(await mapFunction));
}

function getTableName(attribute) {
    return attribute == KEY_DELTA ? "WaterLevelHistory" : "GroundDataHistory";
}
