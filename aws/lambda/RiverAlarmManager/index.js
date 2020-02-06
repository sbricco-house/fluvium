/**
 * Payload:
 *      river : String 
 * 
 * this lambda controls river state. Each river has an attribute called riverWatchPolicy that tells what "policy" need to be called to check the river 
 * state (pattern strategy).
 * 
 */
const AWS = require("aws-sdk")
const response = require("./response.js")
//AMAZON SETUP
AWS.config.update({region: 'eu-west-2'})
const lambda = new AWS.Lambda({apiVersion: '2015-03-31'});
const iot = new AWS.Iot({apiVersion: '2015-05-28'})

exports.handler = async (event) => {
    try {
        let description = await iot.describeThingGroup({thingGroupName : event.river}).promise() //retrieve river group description
        let attributes = description.thingGroupProperties.attributePayload.attributes
        if(attributes.riverWatchPolicyName === undefined || attributes.riverWatchPolicyName === "none") {
            return response.ok("no policy") //No policy, no check on river, return ok with "no policy"
        } else {
            //invoke the policy put in the attributes, passing the entire river object
            let result = await lambda.invoke({
                FunctionName: attributes.riverWatchPolicyName,
                Payload : JSON.stringify(description)
            }).promise()
            return response.ok(result)
        }
    } catch(err) {
        return response.internal("error on alarm managent" + err)
    }
};
