/**
 * this script allow to create a river group
 */
const creation = require("./iot_creation")
const io = require('./io.js');

var riverName = null
var riverLength = null
var policyName = null
var riverImage = null
var description = null
io.question("[CREAZIONE FIUME]; inserisci il nome: ")
    .then(name => riverName = name)
    .then(data => io.question("inserisci la lughezza (in km): "))
    .then(length => riverLength = length)
    .then(data => io.question("inserisci la policy dell'allarme: "))
    .then(alarm => policyName = alarm)
    .then(data => io.question("inserisci l'url di un'imagine: "))
    .then(image => riverImage = image)
    .then(data => io.question("insersci una descrizione: "))
    .then(desc => description = desc)
    .then(data => creation.createRiverGroup(riverName, riverLength, riverImage, policyName, description))
    .catch(error => console.log(error))
    .finally(end => io.readLine.close())
    
    