
function buildResponse(code, data) {
    return {
        statusCode: code,
        body: data
    };
}

exports.response200 = function(data = {}) {
    return buildResponse(200, data);
};
    
exports.response400 = function(msg = "") {
    return buildResponse(400, msg);
};

exports.response404 = function(msg = "") {
    return buildResponse(404, msg);
};

exports.response500 = function(msg = "") {
    return buildResponse(500, msg);
}