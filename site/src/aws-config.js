const AWS = require("aws-sdk");
const AWSMqttClient = require('aws-mqtt')

// Inizializza il provider di credenziali Amazon Cognito
AWS.config.region = 'eu-west-2'; // Regione
AWS.config.credentials = new AWS.CognitoIdentityCredentials({
    IdentityPoolId: 'eu-west-2:af032f7f-f67d-4a27-8351-b12d81fc619a',
});

const lambda = new AWS.Lambda({apiVersion: '2015-03-31'});
const mqttClient = new AWSMqttClient({
    region: AWS.config.region,
    credentials: AWS.config.credentials,
    endpoint: 'a1l0qetj8lwb0i-ats.iot.eu-west-2.amazonaws.com',
    expires: 1, // Sign url with expiration of 600 seconds
    clientId: 'mqtt-client-' + (Math.floor((Math.random() * 100000) + 1)), // clientId to register with MQTT broker. Need to be unique per client
    will: {
        topic: 'WillMsg',
        payload: 'Connection Closed abnormally..!',
        qos: 0,
        retain: false
    } 
})

export default {
    lambda : lambda,
    mqttClient : mqttClient
};