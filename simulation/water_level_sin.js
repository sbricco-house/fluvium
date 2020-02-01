const simulatation = require("./simulation")

if(process.argv.length < 7) {
    console.log("pass: [name] [lat] [long] [max_delta] [std_value]")
    return
}
var lat = parseFloat(process.argv[3])
var long = parseFloat(process.argv[4])
var maxDelta = parseFloat(process.argv[5])
var standardValue = parseFloat(process.argv[6])

console.log("DELTA = " + maxDelta)
console.log("STANDARD VALUE = " + standardValue)
function sinValue(iterationCount) {
    let waterLevelDelta = standardValue + Math.sin(iterationCount / 5) * maxDelta
    return simulatation.awsDataCreation(lat, long,
        {
            water_level : {
                delta: waterLevelDelta,
                timestamp: Date.now()
            }
        }
    )
}

simulatation.simulate(sinValue, 300000) //each five minutes..