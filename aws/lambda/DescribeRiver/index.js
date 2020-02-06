/**
 * Payload required: 
 *      river : String
 * Response(in body):
 *  "deltaLevelAvg": Number,
 *  "imageSrc": Url ,
 *  lastUpdate: Number,
 *  length: Number,
 *  rainQuantityAvg: Number,
 *  riverWatchPolicyName : LambdaName, None,
 *  state: {ok, allerta, allarme},
 *  devices: Array of Device
 *  description : String
 * 
 *  Device : {
 *      name : String,
 *      data : Object,
 *      metaData : Object
 *  }
 * Example: 
 * {
 *      river : "cesano"
 * }
 * this lambda returns the information about a river.
 */
const AWS = require("aws-sdk")
const response = require("./response.js")
//AMAZON SETUP
AWS.config.update({region: 'eu-west-2'})
const iot = new AWS.Iot({apiVersion: '2015-05-28'})
var iotData = new AWS.IotData({
  endpoint: 'a1l0qetj8lwb0i-ats.iot.eu-west-2.amazonaws.com',
  apiVersion: '2015-05-28'
});
exports.handler = async (event) => {
    if(event.river === undefined) {
        return response.badRequest("river missing")
    }
    let name = event.river
    try {
        let description = await iot.describeThingGroup({thingGroupName : name}).promise() //retrieve river name from thing group
        let devices = await iot.listThingsInThingGroup({thingGroupName: name}).promise() //retrieve devices installed on river
        let devicesInResponse = []
        //create of device list in the response
        for (const device of devices.things) {
            let shadow = await iotData.getThingShadow({thingName: device}).promise() //for each device, get the shadow of current state
            let payload = JSON.parse(shadow.payload) //payload is in byte. JSON.parse converts it in json object
            let thingDescription = await iot.describeThing({thingName : device}).promise() //retrieve thing attributes stored on aws iot
            devicesInResponse.push({
                name : device,
                data : payload.state.reported,
                metaData : thingDescription.attributes
            })
        }
        
        let prop = description.thingGroupProperties //river properties
        let payload = {} //empty payload
        //FLAT RESPONSE OBJECT, PUT ATTRIBUTES AT ROOT LEVEL.
        for (const key in prop.attributePayload.attributes) { //put each river attributes into the response
            payload[key] = prop.attributePayload.attributes[key]               
        }
        payload.devices = devicesInResponse //put device installed
        payload.description = prop.thingGroupDescription //put river description
    
        return response.ok(payload)
    } catch (err) {
        response.internal("internal error : " + err)
    }
    
};
