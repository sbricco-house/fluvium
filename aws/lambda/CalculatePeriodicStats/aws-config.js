const AWS = require("aws-sdk")
AWS.config.update({region: 'eu-west-2'})
const dbClient = new AWS.DynamoDB.DocumentClient();

const iot = new AWS.Iot({apiVersion: '2015-05-28'});

module.exports.dbClient = dbClient;
module.exports.iot = iot;