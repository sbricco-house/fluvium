const simulatation = require("./simulation")
const connection = require("./aws_connection")

const map = (value, x1, y1, x2, y2) => (value - x1) * (y2 - x2) / (y1 - x1) + x2;
if(process.argv.length < 6) {
    console.log("pass: [name] [base_water] [max_water] [period]")
    return
}
var period = parseInt(process.argv[5])
var baseWater = parseFloat(process.argv[3])
var maxWater = parseFloat(process.argv[4])

console.log("base Water = " + baseWater)
console.log("max Water = " + maxWater)
function sinValue(iterationCount) {
    let rainQauntity = baseWater + Math.random() * (maxWater - baseWater)
    return connection.dataCreation(
        {
            ground : {
                soil_moisture: map(rainQauntity, 0, maxWater, 0 , 100),
                rain_quantity: rainQauntity,
                timestamp: Date.now()
            }
        }
    )
}

simulatation.simulate(sinValue, period, process.argv[2])