const connection = require("../aws_connection")
const DYNAMO_DELAY = 500
const WRITE_CAPACITY_UNIT = 5
const HALF_HOUR = 1800000

function dataCreation(river, deviceName, data) {
    return {
        PutRequest : {
            Item : {
                "DeviceName" : deviceName,
                "River" : river,
                "Data" : data
            }
        }
    }
}

function bulkSimulation(from, to, deltaTime,  targetTable, logic) { 
    setTimeout(bulkSimulationLogic, DYNAMO_DELAY, from, to, deltaTime, targetTable, logic)
}

async function bulkSimulationLogic(from, to, deltaTime, targetTable, logic) {
    if(from > to) {
        return;
    }
    var elements = []
    for(i = 0; i < WRITE_CAPACITY_UNIT; i ++) {
        var computed = logic()
        computed.PutRequest.Item.Timestamp = from
        elements.push(computed)
        from += deltaTime    
    }
    let result = await connection.dynamo.batchWrite({
        RequestItems : {
            [targetTable] : elements   
        }
    }).promise()
    console.log("store items.. wait 1 second")
    setTimeout(bulkSimulationLogic, DYNAMO_DELAY, from, to, deltaTime, targetTable, logic);
}

function timeIntervalSimulation(type, targetTable, logic) {
    type = type.toUpperCase()
    if(type !== "MONTH" && type !== "DAY" && type !== "WEEK") {
        console.log("wrong time interval, this function supports : MONTH, DAY, WEEK")
        return
    }
    let to = Date.now()
    let from = new Date(to)
    switch (type) {
        case "MONTH": from.setMonth(from.getMonth() - 1)
        break
        case "DAY": from.setDate(from.getDate() - 1)
        break
        case "WEEK": from.setDate(from.getDate() - 7)
        break
    }
    bulkSimulation(from.getTime(), to, HALF_HOUR, targetTable, logic)
}

module.exports.timeIntervalSimulation = timeIntervalSimulation
module.exports.dataCreation = dataCreation