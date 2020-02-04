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
                            :icon="stateInfo.icon"
                            :color="stateInfo.color" />
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
//TODO Create river info component
import StatCard from "@/components/StatsCard.vue"
import DeviceMap from '@/components/DeviceMap.vue'
import DevicePage from '@/components/DevicePage.vue'
import river from "@/model/river.js"
import aws from "@/services/aws-lambda.js"
import mqtt from "@/services/aws-mqtt.js"

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
    data : () => ({
        river : river.default,
    }),
    computed : {
        deltaAvg : function() { return parseFloat(this.river.deltaLevelAvg).toFixed(2) + " m" },
        rainQuantityAvg : function() { return parseFloat(this.river.rainQuantityAvg).toFixed(2) + " mm" },
        length : function() { return this.river.length  + " km" },
        devicesInAllarmCount() {
            let allarmCount = this.river.devices.filter(dev => dev.metaData.alarm === "true").length
            return "dispositivi in allarme : " + allarmCount
        },
        stateInfo : function() {
            switch(this.river.state) {
                case "allerta" : return { icon : "mdi-alert", color : "warning" }
                case "pericolo" : return { icon : "mdi-alert-decagram", color : "error" }
                default : return { icon : "mdi-check-all", color : "success"}
            }
        }

    },
    methods : {
        onClickDevice : function(device) { this.$refs.devicePage.open(device); }
    },
    mounted() {
        //subscribe on river topics
        aws.executeLambda("DescribeRiver", {river: this.riverName}).then(rec => this.river = rec.data.body)
        mqtt.subscribeRiverNotification(
            this.riverName,
            payload => this.river.state = payload.state,
            payload => this.river.devices.filter(dev => dev.name === payload.name)
                                        .forEach(dev => dev.metaData.alarm = payload.state)
        )
    },
    destroyed() {
        mqtt.unsubscribeRiverNotification(this.riverName)
    }
}
</script>