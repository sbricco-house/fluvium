
import lambda from "@/aws-config"

let aws = {
    executeLambda: function(functionName, payloadArgs) {
        let params = {
            FunctionName : functionName,
            InvocationType : 'RequestResponse',
            LogType : 'None',
            Payload: payloadArgs
        };
    
        return new Promise((accept, reject) => {
            lambda.invoke(params, (err, data) => {
                if(!err) { 
                    accept({
                        status: data.StatusCode,
                        data: JSON.parse(data.Payload)
                    });
                } else {
                    reject(err);
                }
            });
        })
    }
}

export default aws;