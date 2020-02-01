<template>
    <v-dialog v-model="dialog" fullscreen hide-overlay transition="dialog-bottom-transition">
        <v-card v-if="dialog">
            <v-toolbar dark color="secondary">
            <v-btn icon dark @click="close">
                <v-icon>mdi-close</v-icon>
            </v-btn>
        
            <v-toolbar-title>{{device.name}}</v-toolbar-title>
            <v-icon class="ml-2">mdi-bug</v-icon>
            
            <v-spacer></v-spacer>
            </v-toolbar>
            <template v-if="isWaterLevel(device)">
                <water-level-device :device="device"/>
            </template>
            <template v-else>
                <ground-device :device="device"/>
            </template>
            
        </v-card>
    </v-dialog>
</template>

<script>
import WaterLevelDevice from "@/components/WaterLevelDevice.vue"
import GroundDevice from "@/components/GroundDevice.vue"
import aws from "@/services/aws-mqtt.js"
export default {
    name : "device-page",
    data () {
        return {
            device : null,
            dialog : false    
        }
    },
    components : {
        "water-level-device" : WaterLevelDevice,
        'ground-device' : GroundDevice
    },
    methods : {
        open : function(device){
            this.device = device,
            aws.subscribeShadow(device.name, (shadow) => {
                console.log("SHADOW UPDATED : = " + JSON.stringify(shadow))
                if(this.isWaterLevel(device)) {
                    device.data.water_level = shadow.state.reported.water_level
                } else {
                    device.data.ground = shadow.state.reported.ground
                }
            })
            this.dialog = true
        },
        close : function() {
            this.dialog = false
            aws.unsubscibeShadow(this.device.name)
        },
        isWaterLevel : function(device) {
            return device.name.includes("waterlevel")
        }
    }
}
</script>