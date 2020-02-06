/**
 * get attributes from a thing.
 * {iot} : aws iot object used to interect with aws.
 * {thingName} : the name of device.
 */
async function getAttributes(iot, thingName) {
    var params = {
        thingName: thingName 
    };
    var thing = await iot.describeThing(params).promise()
    return thing.attributes;
}
/**
 * get the river in which the device is installed.
 * {iot} : aws iot object used to interect with aws.
 * {thingName} : the name of device.
 */
async function getRiver(iot, thingName) {
    var params = {
      thingName: thingName, /* required */
      maxResults: 1,
    }
    let group = await iot.listThingGroupsForThing(params).promise()
    if(group.thingGroups[0] === undefined) {
        return null
    } else {
        return group.thingGroups[0].groupName
    }
}

module.exports.getAttributes = getAttributes
module.exports.getRiver = getRiver
