let AWS = require("aws-sdk")
AWS.config.update({
    region: 'eu-west-3' //region where services are installed
    //credential must be set in $home/.aws/credentials
})
//iot framework used to create things in aws iot
const iot = new AWS.Iot({apiVersion: '2015-05-28'})
//shadow framework used to update thing internal data
const shadow = new AWS.IotData({
    apiVersion: '2015-05-28',
    endpoint: 'a1l0qetj8lwb0i-ats.iot.eu-west-3.amazonaws.com'
})

module.exports.iot = iot
module.exports.shadow = shadow