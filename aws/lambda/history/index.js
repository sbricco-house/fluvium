//IMPORT
const AWS = require("aws-sdk")
const response = require("./response.js")
const validation = require("./data_validation.js")
const attributesRetriever = require("./attributes_retriever.js")
const waterLevelPreprocess = require("./water_level_preprocess.js")
const uploader = require("./upload_data.js")
const alarm = require("./alarm_management.js")
//AMAZON SETUP
AWS.config.update({region: 'eu-west-2'})
const client = new AWS.DynamoDB.DocumentClient()
const iot = new AWS.Iot({apiVersion: '2015-05-28'})
const lambda = new AWS.Lambda()
const iotData = new AWS.IotData({
    endpoint: 'a1l0qetj8lwb0i-ats.iot.eu-west-2.amazonaws.com',
    apiVersion: '2015-05-28'
})
/**
 * Payload : {
 *      thing_name: String ,
 *      state: WaterLevelType or GroundType
 * }
 * 
 * WaterLevelType : {
 *      reported: {
 *          waterlevel : {
 *              timestamp : Number,
 *              delta : Number
 *          }
 *      }    
 * }
 * 
 * GroundType : {
 *      reported : {
 *          ground : { 
 *              timestamp : Number,
 *              rain_quantity : Number,
 *              soil_moisture : Number
 *          }
 *      }
 * }
 */
exports.handler = async (event, ctx, callback) => {
    /* checks payload correctness */
    //NO NAME SET IN REQUEST
    if(event.thing_name === undefined) {
        return response.notFound("name missed")
    }
    var attributes = null
    try {
        attributes = await attributesRetriever.getAttributes(iot, event.thing_name)
    } catch(err) {
        return response.internal("problem during attributes retriving")
    }
    
    var riverName = null
    try {
        riverName = await attributesRetriever.getRiver(iot, event.thing_name)
    } catch(err) {
        return response.internal("problem during river name retriving" + err)
    }
    if(riverName == null) {
        return response.notFound("river group not set for device")
    }
    attributes.river = riverName
    //WRONG BODY RECEIVED
    if(event.state === undefined || event.state.reported === undefined) {
        return response.badRequest("wrong body format")
    }
    let body = event.state.reported
    var entry = null //entry to put into the dynamo db
    
    if(validation.isWaterLevelData(body)) {
        //adjust water level data using attributes
        let adjusted = waterLevelPreprocess.adjust(body, attributes)
        try {
            //verify if some allarm are triggered with this new data.
            await alarm.verify(iot, iotData, event.thing_name, adjusted, attributes)
        } catch (err) {
            return response.internal("error on alarm verification:" + err)
        }
        entry = uploader.createDynamoWaterData(event.thing_name, adjusted, attributes)
    } else if (validation.isGroudData(body)) {
        entry = uploader.createDynamoGroundData(event.thing_name, body, attributes)
    } else {
        return response.badRequest("body type not found");
    }
    //upload the data on dynamodb (in the correct table).
    try {
        let result = await uploader.uploadOnDynamo(client, entry)
        let alarmManagerResult = await lambda.invoke({
                FunctionName: "RiverAlarmManager",
                Payload : JSON.stringify({river : riverName})
        }).promise()
        return response.ok(result)
    } catch (err) {
        return response.internal("error on upload data" + err)
    }
    
};
