const AWS = require("aws-sdk");
const AWSMqttClient = require('aws-mqtt')
// Inizializza il provider di credenziali Amazon Cognito
AWS.config.region = 'eu-west-2'; // Regione
AWS.config.credentials = new AWS.CognitoIdentityCredentials({
    IdentityPoolId: 'eu-west-2:af032f7f-f67d-4a27-8351-b12d81fc619a',
});

const lambda = new AWS.Lambda({region: 'eu-west-2', apiVersion: '2015-03-31'});
//TODO 
const client = new AWSMqttClient({
    region: AWS.config.region,
    credentials: AWS.config.credentials,
    endpoint: 'a1l0qetj8lwb0i-ats.iot.eu-west-2.amazonaws.com',
    expires: 600, // Sign url with expiration of 600 seconds
    clientId: 'mqtt-client-browser' 
})

client.on('connect', () => {
    console.log("here..")
    //client.subscribe('/myTopic')
})
export default lambda;