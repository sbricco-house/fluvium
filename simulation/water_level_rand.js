const simulatation = require("./simulation")

if(process.argv.length < 8) {
    console.log("pass: [name] [river] [lat] [long] [max_delta] [std_value]")
    return
}
var lat = process.argv[4]
var long = process.argv[5]
var maxDelta = parseFloat(process.argv[6])
var standardValue = parseFloat(process.argv[7])

console.log("DELTA = " + maxDelta)
console.log("STANDARD VALUE = " + standardValue)
function randomValue(iterationCount) {
    let waterLevelDelta = standardValue + (Math.random() * (2*maxDelta) - maxDelta)
    return simulatation.awsDataCreation(lat, long,
        {
            water_level : {
                delta: waterLevelDelta,
                timestamp: Date.now()
            }
        }
    )
}

simulatation.simulate(randomValue, 10000)