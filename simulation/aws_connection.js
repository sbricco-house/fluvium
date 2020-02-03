let AWS = require("aws-sdk")
AWS.config.update({
    region: 'eu-west-2' //region where services are installed
    //credential must be set in $home/.aws/credentials
})
//iot framework used to create things in aws iot
const iot = new AWS.Iot({apiVersion: '2015-05-28'})
//dynamo framewoek used to put elements on dynamodb
const dynamo = new AWS.DynamoDB.DocumentClient()
//shadow framework used to update thing internal data
const shadow = new AWS.IotData({
    apiVersion: '2015-05-28',
    endpoint: 'a1l0qetj8lwb0i-ats.iot.eu-west-2.amazonaws.com'
})

module.exports.iot = iot
module.exports.shadow = shadow
module.exports.dynamo = dynamo
