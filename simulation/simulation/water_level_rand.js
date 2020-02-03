const simulatation = require("./simulation")

if(process.argv.length < 6) {
    console.log("pass: [name] [max_delta] [std_value] [period]")
    return
}
var period = parseInt(process.argv[5])
var maxDelta = parseFloat(process.argv[3])
var standardValue = parseFloat(process.argv[4])

console.log("DELTA = " + maxDelta)
console.log("STANDARD VALUE = " + standardValue)
console.log("PERIOD = " + period)
function randomValue(iterationCount) {
    let waterLevelDelta = standardValue + (Math.random() * (2*maxDelta) - maxDelta)
    return simulatation.dataCreation(
        {
            water_level : {
                delta: waterLevelDelta,
                timestamp: Date.now()
            }
        }
    )
}
simulatation.simulate(randomValue, period, process.argv[2])