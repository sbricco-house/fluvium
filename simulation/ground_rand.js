const simulatation = require("./simulation")
const map = (value, x1, y1, x2, y2) => (value - x1) * (y2 - x2) / (y1 - x1) + x2;
if(process.argv.length < 7) {
    console.log("pass: [name] [lat] [long] [base_water] [max_water]")
    return
}
console.log("ARG LEN = " + process.argv.length)
var lat = process.argv[3]
var long = process.argv[4]
var baseWater = parseFloat(process.argv[5])
var maxWater = parseFloat(process.argv[6])

console.log("base Water = " + baseWater)
console.log("max Water = " + maxWater)
function sinValue(iterationCount) {
    let rainQauntity = baseWater + Math.random() * (maxWater - baseWater) + baseWater
    return simulatation.awsDataCreation(lat, long,
        {
            ground : {
                soil_moisture: map(rainQauntity, 0, maxWater, 0 , 100),
                rain_quantity: rainQauntity,
                timestamp: Date.now()
            }
        }
    )
}

simulatation.simulate(sinValue, 10000)