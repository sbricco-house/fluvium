/**
 * Payload : {
 *      river description received from aws iot
 * }
 * Return : {
 *      "no changes" or "river change state in {ok, allarme, pericolo}"
 * }
 * this is a simple policy used to manage river state:
 *  the river goes to:
 *      ok : if there aren't devices in alarm state.
 *      allarme : if there is a device in alarm state.
 *      pericolo : if there are three devices in alarm state.
 */

const AWS = require("aws-sdk")
const response = require("./response.js")
//AMAZON SETUP
AWS.config.update({region: 'eu-west-2'})
const iot = new AWS.Iot({apiVersion: '2015-05-28'})
const iotData = new AWS.IotData({
    endpoint: 'a1l0qetj8lwb0i-ats.iot.eu-west-2.amazonaws.com',
    apiVersion: '2015-05-28'
})

//river notification publish on aws iot (using mqtt)
function riverStateNotification(riverName, state) {
    return {
        topic : `/river/${riverName}/state`,
        payload : JSON.stringify({state : state})
    }
}

exports.handler = async (event) => {
    let description = event
    let riverName = description.thingGroupName
    let attrPayload = description.thingGroupProperties.attributePayload
    attrPayload.merge = true
    let attributes = attrPayload.attributes
    let riverState = attributes.state
    //retrieve all device installed on river
    let things = await iot.listThingsInThingGroup({thingGroupName: riverName}).promise()
    //count device in alarm state
    let alarmCount = 0;
    for(let thing of things.things) {
        let thingDescription = await iot.describeThing({thingName : thing}).promise()
        alarmCount += thingDescription.attributes.alarm === 'true' ? 1 : 0
    }
    //verify a river state change
    let oldState = attributes.state
    if(alarmCount >= 1) {
        attributes.state = "allerta"
    } else if(alarmCount > 3) {
        attributes.state = "pericolo"
    } else if (alarmCount === 0) {
        attributes.state = "ok"
    }
    
    if(oldState !== attributes.state) {
        try {
            //update new state on aws iot group
            let result = await iot.updateThingGroup({
                thingGroupName : riverName,
                thingGroupProperties : description.thingGroupProperties
            }).promise()
            //publish new state on topic /river/{riverName}/state
            await iotData.publish(riverStateNotification(riverName, attributes.state)).promise()
            return response.ok("river change state in : " +  attributes.state)
        } catch(error) {
            return response.internal("error on change river state : " + error)
        }        
    }
    
    return response.ok("no changes")
};
