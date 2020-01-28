/*
* base structure used to simulate a fluvium device.
*/
//TODO Add gps information
const connection = require("./aws_connection")

//creation of basic structure of iot shadow data
function awsDataCreation(lat, long,data) {
    data.gps = {
        lat : lat,
        long : long,
        altitude : 0,
        hdop : 1,
        fixTimestamp : Date.now()
    }
    return {
        state : {
            reported : data
        }
    }
}

/*
simulation publishing data at fixed time (period.).
simulationLogic describe how to create data.
*/
async function simulate(simulationLogic, period) {
    if(process.argv[2] == undefined) {
        console.log("error, name or river missing, pass : [deviceName] [river]")
    }
    let name = process.argv[2]
    var iterationCount = 0
    //simulation loop
    setInterval(() => {
        var data = simulationLogic(iterationCount) //take data.
        iterationCount += 1
        let topic = "$aws/things/" + name + "/shadow/update"
        console.log("TOPIC = " + topic)
        console.log("DATA = " + JSON.stringify(data))
        //publish data to aws service
        connection.shadow.publish({
            topic : topic,
            payload: JSON.stringify(data),
        })
        .promise()
        .then(result => console.log("updated = " + JSON.stringify(result)))
        .catch(error => console.log(error))
    }, period);
}

module.exports.simulate = simulate
module.exports.awsDataCreation = awsDataCreation