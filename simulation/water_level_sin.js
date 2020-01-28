const simulatation = require("./simulation")

if(process.argv.length < 7) {
    console.log("pass: [name] [lat] [long] [max_delta] [std_value]")
    return
}
var lat = process.argv[3]
var long = process.argv[4]
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

simulatation.simulate(sinValue, 10000)