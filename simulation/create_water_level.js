/**
 * this script create a device that sense the river water level
 */
const creation = require("./iot_creation")
const io = require('./io.js');

var deviceName = null
var zero = null
var thrAlarm = null
var thrUnalarm = null
var riverName = null
io.question("[CREAZIONE DISPOSITIVO]; inserisci il nome: ")
    .then(name => deviceName = name)
    .then(data => io.question("inserisci il fiume dov'è installato: "))
    .then(river => riverName = river)
    .then(data => io.question("inserisci la distanza corrispondente allo zero: "))
    .then(z => zero = z)
    .then(data => io.question("inserisci l'altezza d'allarme: "))
    .then(alarm => thrAlarm = alarm)
    .then(data => io.question("inserisci l'altezza per uscire dall'allarme: "))
    .then(unalarm => thrUnalarm = unalarm)
    .then(data => {
        let attributes = {
            zero : zero,
            alarm : "false",
            thresholdAlarm : thrAlarm,
            thresholdSafe : thrUnalarm
        }
        return creation.createThing(deviceName, riverName, attributes, "WaterLevelType")
    })
    .catch(error => console.log(error))
    .finally(end => io.readLine.close())