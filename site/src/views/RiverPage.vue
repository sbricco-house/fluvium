<template>
    <v-container fluid>
        <device-page ref="devicePage"/>
        <v-row class="mt-n2">
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card name="Fiume" 
                            :description="riverName | capitalize"
                            icon="mdi-waves"
                            color="grey"/>
            </v-col>
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Stato"
                            :description="river.state | capitalize"
                            :icon="iconState"
                            :color="colorState" />
            </v-col>
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Lunghezza"
                            :description="length"
                            icon="mdi-road-variant"
                            color="brown"/>
            </v-col>

            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Dispositivi"
                            :description="river.devices.length.toString()"
                            icon="mdi-bug"
                            color="grey"
                            footerIcon="mdi-information"
                            :footerDescription="devicesInAllarmCount" />
            </v-col>
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Inalzamento medio"
                            :description="deltaAvg"
                            icon="mdi-swap-vertical-bold"
                            color="red"
                            footerIcon="mdi-history"
                            footerDescription="aggiornato : adesso" />
            </v-col>
                
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Acqua caduta"
                            :description="rainQuantityAvg"
                            icon="mdi-water"
                            color="blue"
                            footerIcon="mdi-history"
                            footerDescription="aggiornato : adesso"/>
            </v-col>
        </v-row>
        <v-row class="">
            <v-col cols="12">
                <v-card elevation="6">    
                    
                    <device-map :devices="river.devices" v-on:onDeviceSelected="onClickDevice"></device-map>
                </v-card>
            </v-col>
        </v-row>
    </v-container>
</template>

<script>
import StatCard from "@/components/StatsCard.vue"
import DeviceMap from '@/components/DeviceMap.vue'
import DevicePage from '@/components/DevicePage.vue'

import aws from "@/services/aws-lambda.js"

export default {
    name : "river-page",
    props : {
        riverName : String
    },
    components : {
        "stat-card" : StatCard,
        "device-page" : DevicePage,
        "device-map" : DeviceMap
    },
    data () {
        return {
            river : {
                deltaLevelAvg : 0,
                rainQuantityAvg : 0,
                length : 0,
                description : "",
                state : "ok",
                devices : []
            },
        }
    },
    computed : {
        deltaAvg : function() { return this.river.deltaLevelAvg + " m" },
        rainQuantityAvg : function() { return this.river.rainQuantityAvg + " mm"},
        length : function() {return this.river.length  + " km"},
        devicesInAllarmCount() {
            let allarmCount = this.river.devices.filter(dev => dev.metaData.alarm === "true").length
            return "dispositivi in allarme : " + allarmCount
        },
        colorState : function() {
            if(this.river.state === "allerta") {
                return "warning"
            } else if(this.river.state === "pericolo") {
                return "error"
            } else {
                return "success"
            }
        },
        iconState : function() {
            if(this.river.state === "allerta") {
                return "mdi-alert"
            } else if(this.river.state === "pericolo") {
                return "mdi-alert-decagram"
            } else {
                return "mdi-check-all"
            }
        }
    },
    methods : {
        onClickDevice : function(device) {
            this.$refs.devicePage.open(device);
        }
    },
    mounted() {
        aws.executeLambda("DescribeRiver", {river: this.riverName}).then(rec => this.river = rec.data.body)
    }
}
</script>