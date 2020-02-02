const simulatation = require("./bulk_simulation")
if(process.argv.length < 6) {
    console.log("pass: [name] [river] [table] [time_interval] [max_delta] [std_value] ")
    return
}
//TODO FIND A WAY TO MANAGE THIS ELEMENTS
var name = process.argv[2]
var river = process.argv[3]
var table = process.argv[4]
var timeInterval = process.argv[5]
var maxDelta = parseFloat(process.argv[6])
var standardValue = parseFloat(process.argv[7])

console.log("DELTA = " + maxDelta)
console.log("STANDARD VALUE = " + standardValue)
console.log("TYPE = " + timeInterval)

function randomValue() {
    let waterLevelDelta = standardValue + (Math.random() * (2*maxDelta) - maxDelta)
    return simulatation.dataCreation(river, name,
        {
            delta: waterLevelDelta
        }
    )
}
simulatation.timeIntervalSimulation(timeInterval, table, randomValue)