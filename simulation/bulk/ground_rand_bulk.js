const simulatation = require("./bulk_simulation")
const util = require("../util.js")
if(process.argv.length < 6) {
    console.log("pass: [name] [river] [table] [time_interval] [base_water] [max_water] ")
    return
}
//TODO FIND A WAY TO MANAGE THIS ELEMENTS
var name = process.argv[2]
var river = process.argv[3]
var table = process.argv[4]
var timeInterval = process.argv[5]
var baseWater = parseFloat(process.argv[6])
var maxWater = parseFloat(process.argv[7])

console.log("BASE WATER = " + baseWater)
console.log("MAX WATER = " + maxWater)
console.log("TYPE = " + timeInterval)

function randomValue() {
    let rainQauntity = baseWater + Math.random() * (maxWater - baseWater)
    return simulatation.dataCreation(river, name,
        {
            soil_moisture: util.map(rainQauntity, baseWater, maxWater, 0 , 100),
            rain_quantity: rainQauntity
        }
    )
}
simulatation.timeIntervalSimulation(timeInterval, table, randomValue)