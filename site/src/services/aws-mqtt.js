import awsElements from "@/aws-config"

let subscriptions = {} //subscription map: link topic to callback
//TODO docu
awsElements.mqttClient.on("message", function(topic, message){ 
    console.log(JSON.parse(message))
    if(subscriptions[topic] != undefined) {
        subscriptions[topic](JSON.parse(message))
    }
})
let mqtt = {
    subscribeShadow : function(name, callback) {
        awsElements.mqttClient.subscribe(`$aws/things/${name}/shadow/update`)
        subscriptions[`$aws/things/${name}/shadow/update`] = callback
    },
    unsubscibeShadow : function(name) {
        awsElements.mqttClient.subscribe(`$aws/things/${name}/shadow/update`)
        subscriptions[`$aws/things/${name}/shadow/update`] = undefined
    },
    subscribeRiverNotification(name, changeRiverState, changeDeviceState) {
        awsElements.mqttClient.subscribe(`/river/${name}/state`)
        awsElements.mqttClient.subscribe(`/river/${name}/devices`)
        subscriptions[`/river/${name}/state`] = changeRiverState
        subscriptions[`/river/${name}/devices`] = changeDeviceState
    
    },
    unsubscribeRiverNotification(name) {
        awsElements.mqttClient.unsubscribe(`/river/${name}/state`)
        awsElements.mqttClient.unsubscribe(`/river/${name}/devices`)
        subscriptions[`/river/${name}/state`] = undefined
        subscriptions[`/river/${name}/devices`] = undefined
    }
}
export default mqtt