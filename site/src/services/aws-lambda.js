
import awsElements from "@/aws-config"

let lambda = {
    executeLambda: function(functionName, payloadArgs) {
        let params = {
            FunctionName : functionName,
            InvocationType : 'RequestResponse',
            LogType : 'None',
            Payload: JSON.stringify(payloadArgs)
        };
    
        return new Promise((accept, reject) => {
            awsElements.lambda.invoke(params, (err, data) => {
                if(!err) {
                    console.log(data)
                    accept({
                        status: data.StatusCode,
                        data: JSON.parse(data.Payload)
                    });
                } else {
                    console.log(err)
                    reject(err);
                }
            });
        })
    }
}

export default lambda;