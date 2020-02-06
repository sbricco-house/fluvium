async function uploadOnDynamo(dbClient, data) {
    return await dbClient.put(data)
                    .promise()
}

function createEntry(tableName, deviceName, timestamp, river, data) {
    return {
        TableName: tableName,
        Item : {
            'DeviceName' : deviceName,
            'Timestamp' : timestamp,
            'River' : river,
            'Data' : data
        }
    }
}
function createDynamoGroundData(name, data, attributes) {
    var groundData = {
        soil_moisture : data.ground.soil_moisture,
        rain_quantity : data.ground.rain_quantity
    }
    return createEntry("GroundDataHistory", name, data.ground.timestamp, attributes.river, groundData)
}

function createDynamoWaterData(name, data, attributes) {
    var waterData = {
        delta : data.water_level.delta
    }
    return createEntry("WaterLevelHistory", name, data.water_level.timestamp, attributes.river, waterData)
}

module.exports.createDynamoWaterData = createDynamoWaterData
module.exports.createDynamoGroundData = createDynamoGroundData
module.exports.uploadOnDynamo = uploadOnDynamo
