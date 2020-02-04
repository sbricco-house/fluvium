const connection = require("../aws_connection")

if(process.argv.length < 5) {
    console.log("pass: [name] [lat] [long]")
    return
}

var lat = parseFloat(process.argv[3])
var long = parseFloat(process.argv[4])
var name = process.argv[2]

let topic = "$aws/things/" + name + "/shadow/update"

let data = connection.dataCreation({
    gps : {
        latitude : lat,
        longitude : long,
        altitude : 0,
        hdop : 1,
        fixTimestamp : Date.now()
    } 
})
connection.shadow.publish({
    topic : topic,
    payload: JSON.stringify(data),
})
.promise()
.then(result => console.log("updated = " + JSON.stringify(result)))
.catch(error => console.log(error))
