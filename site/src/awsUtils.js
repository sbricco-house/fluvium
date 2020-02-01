module.exports = {
    createStatRequest : function(name, attribute, timerange) {
        return {
            deviceName: name,
            attribute: attribute,
            timeRange: timerange
        }
    }
}