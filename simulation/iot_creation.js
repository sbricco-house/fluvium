const connection = require("./aws_connection")
var util = require('util')
//return a json used to create a aws iot thing "fluvium" like
function buildCreationParams(name, attributes, type) {
    return {
        thingName : name,
        attributePayload: {
            attributes: attributes,
            merge: false
        },
        thingTypeName: type
    }
}

function buildAddGroupParams(thingName, groupName) {
    return {
        thingGroupName: groupName,
        thingName: thingName
    }
}
function buildGroupParams(groupName, length, imageSrc, riverWatchPolicyName, description) {
    return {
        thingGroupName : groupName,
        thingGroupProperties: {
            attributePayload: {
                attributes : {
                    length : length,
                    imageSrc : imageSrc,
                    deltaLevelAvg : "0",
                    rainQuantityAvg : "0",
                    riverWatchPolicyName : riverWatchPolicyName,
                    state : "ok",
                    timestamp : 0
                },
                merge : false
            },
            thingGroupDescription : description 
        }
    }
}
/*
thing creation used river and name passed
*/
async function createThing(name, river, attributes, type) {
    try {
        await connection.iot.createThing(buildCreationParams(name, attributes, type)).promise()
        await connection.iot.addThingToThingGroup(buildAddGroupParams(name, river)).promise()
    } catch (err) {
        console.log(err)
    }
}

async function createRiverGroup(riverName, length, imageSrc, riverWatchPolicyName, description) {
    let groupParams = buildGroupParams(riverName, length, imageSrc, riverWatchPolicyName, description)
    return await connection.iot.createThingGroup(groupParams)
        .promise()
        .then(res => console.log("group created : " + JSON.stringify(res)))
        .catch(err => console.log("error : " + JSON.stringify(err)))
}

module.exports.createThing = createThing
module.exports.createRiverGroup = createRiverGroup