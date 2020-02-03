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
function sinValue(iterationCount) {
    let waterLevelDelta = standardValue + Math.sin(iterationCount / 5) * maxDelta
    return simulatation.dataCreation(
        {
            water_level : {
                delta: waterLevelDelta,
                timestamp: Date.now()
            }
        }
    )
}

simulatation.simulate(sinValue, period, process.argv[2])