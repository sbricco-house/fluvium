const AWS = require("aws-sdk")
AWS.config.update({region: 'eu-west-2'})
const dbClient = new AWS.DynamoDB.DocumentClient();

module.exports.dbClient = dbClient;