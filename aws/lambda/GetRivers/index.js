const AWS = require("aws-sdk")
const response = require("./response.js")
//AMAZON SETUP
AWS.config.update({region: 'eu-west-2'})
const iot = new AWS.Iot({apiVersion: '2015-05-28'})
/*
 * Payload : {}
 * returns : Array of River
 * River : {
 *      river : String,
 *      description : String
 *      imageSrc : Url
 * }
 * this functions return the river list stored on aws iot
 *
*/
exports.handler = async (event) => {
    try {
        let groups = await iot.listThingGroups({}).promise() //returns all rivers
        const mapped = groups.thingGroups.map(group => group.groupName) //map the groups to river names
        let rivers = [] //response.
        for (const riverName of mapped) {
            let description = await iot.describeThingGroup({thingGroupName : riverName}).promise() //retrieve the river description from aws iot
            let attributesGroup = description.thingGroupProperties.attributePayload.attributes 
            rivers.push({
                river : riverName,
                description : description.thingGroupProperties.thingGroupDescription,
                imageSrc : attributesGroup.imageSrc
            })
        }
        return response.ok(rivers)
    } catch(err) {
        return response.internal("error on retriving group : " + err)
    }
};
