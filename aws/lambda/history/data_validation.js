/**
 * tells if the body contains water level data
 * return : true if the body contains attributes "water_level" false otherwhise
 */
function isWaterLevelData(body) {
    return body.hasOwnProperty("water_level")
}

/**
 * tells if the body contains ground data
 * return : true if the body contains attributes "ground" false otherwhise
 */
function isGroundData(body) {
    return body.hasOwnProperty("ground")
}

module.exports.isWaterLevelData = isWaterLevelData
module.exports.isGroudData = isGroundData