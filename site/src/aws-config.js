const AWS = require("aws-sdk");

// Inizializza il provider di credenziali Amazon Cognito
AWS.config.region = 'eu-west-2'; // Regione
AWS.config.credentials = new AWS.CognitoIdentityCredentials({
    IdentityPoolId: 'eu-west-2:af032f7f-f67d-4a27-8351-b12d81fc619a',
});

const lambda = new AWS.Lambda({region: 'eu-west-2', apiVersion: '2015-03-31'});

export default lambda;