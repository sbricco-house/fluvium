function notFound(reason) {
    return {
        statusCode : 404,
        body : JSON.stringify(reason)
    }
}

function internal(reason) {
    return {
        statusCode: 500,
        body : JSON.stringify(reason)
    }
}

function badRequest(reason) {
    return {
        statusCode : 400,
        body : JSON.stringify(reason)
    }
}
function ok(result) {
    return {
        statusCode : 200,
        body: result
    }
}

module.exports.notFound = notFound
module.exports.ok = ok
module.exports.internal = internal
module.exports.badRequest = badRequest