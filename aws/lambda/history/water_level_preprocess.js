/**
 * adjust water level data checking the zero value.
 */
function adjust(data, attributes) {
    data.water_level.delta = attributes.zero - data.water_level.delta
    return data
}

module.exports.adjust = adjust