/**
 * this script create a device that sense the ground data
 */
const creation = require("./iot_creation")
const io = require('./io.js');

var deviceName = null
io.question("[CREAZIONE DISPOSITIVO]; inserisci il nome: ")
    .then(name => deviceName = name)
    .then(data => io.question("inserisci il fiume dov'è installato: "))
    .then(riverName => {
        let attributes = {}
        return creation.createThing(deviceName, riverName, attributes, undefined)
    })
    .catch(error => console.log(error))
    .finally(end => io.readLine.close())