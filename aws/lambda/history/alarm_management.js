async function updateAlarm(iot, iotData, thingName, attributes) {
    await iot.updateThing({
        thingName : thingName,
        attributePayload: {
            attributes : attributes,
            merge : true
        }
    }).promise()
    let result = await iotData.publish({
        topic : `/river/${attributes.river}/devices`,
        payload : JSON.stringify({name : thingName, state : attributes.alarm})
    }).promise()
    console.log("RESULT = " + JSON.stringify(result))
}

async function verify(iot, iotData, thingName, data, attributes) {
    if(attributes.alarm === "false" && data.water_level.delta > attributes.thresholdAlarm) {
        attributes.alarm = "true"
        updateAlarm(iot, iotData, thingName, attributes)
    } else if(attributes.alarm === "true" && data.water_level.delta < attributes.thresholdSafe){
        attributes.alarm = "false"
        updateAlarm(iot, iotData, thingName, attributes)
    }
}

module.exports.verify = verify