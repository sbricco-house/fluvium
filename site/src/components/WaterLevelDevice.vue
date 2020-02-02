<template>
    <v-container fluid>
        <v-row class="ml-2 mr-2">
            <v-col cols="12" :sm="6" :md="6" :lg="3">
                <stat-card name="Stato" 
                    :description="state"
                    icon="mdi-information"
                    :color="stateColor" />
            </v-col>
            <v-col cols="12" :sm="6" :md="6" :lg="3">
                <stat-card name="Livello del fiume" 
                            :description="waterLevel"
                            icon="mdi-swap-vertical-bold"
                            color="red"
                            footerIcon="mdi-history"
                            :footerDescription="updatedAt" />
                
            </v-col>
            <v-col cols="12" :sm="6" :md="6" :lg="3">
                <stat-card name="Soglia allarme" 
                            :description="thresholdAlarm"
                            icon="mdi-auto-upload"
                            color="info" />
                
            </v-col>
        </v-row>
        <v-row class="ml-2 mr-2 mt-10">
            <v-col cols="12" :md="12" :lg="6">
                <data-chart color="#00BCD4" 
                            title="Livello acqua" 
                            footerHistory="Ultimo giorno" 
                            icon="mdi-swap-vertical-bold"
                            :dataFormat="formats.hour"
                            :data="levelDay"
                />
            </v-col>
            <v-col cols="12" :md="12" :lg="6">
                <data-chart color="#00BCD4" 
                            title="Livello acqua" 
                            footerHistory="Ultimi 7 giorni" 
                            icon="mdi-swap-vertical-bold"
                            :dataFormat="formats.day"
                            :data="levelWeek"
                />
            </v-col>
            <v-col cols="12" :md="12" :lg="12">
                 <data-chart color="#00BCD4" 
                            title="Livello acqua" 
                            footerHistory="Ultimo mese" 
                            icon="mdi-swap-vertical-bold"
                            :dataFormat="formats.monthAndDay"
                            :data="levelMonth"
                />
            </v-col>
        </v-row>
    </v-container>
</template>

<script>

import StatCard from "@/components/StatsCard.vue"
import DataChart from "@/components/DataChart.vue"
let DateUtils = require("@/dateUtils.js");
import aws from "@/services/aws-lambda.js"
let utils = require("@/awsUtils");
export default {
    name : "water-level-device",
    components : {
        "stat-card" : StatCard,
        'data-chart' : DataChart
    },
    data() {
        return {
            formats : DateUtils,    
            levelDay : [],
            levelWeek : [],
            levelMonth : []
        }
    },
    props : {
        device : Object,
    },
    computed: {
        updatedAt : function() {
            let dateSensed = new Date(this.device.data.water_level.timestamp)
            return "aggiornato : " + dateSensed.toLocaleString('it-IT', DateUtils.fullDate);
        },
        state : function() {
            switch(this.device.metaData.alarm) {
                case "true" : return "In allarme"
                default : return "Nella norma"
            }
        },
        stateColor : function() {
            switch(this.device.metaData.alarm) {
                case "true" : return "warning"
                default : return "success"
            }
        },
        waterLevel : function() { 
            let delta = this.device.data.water_level.delta.toFixed(2)
            let zero = this.device.metaData.zero
            return (zero - delta).toFixed(2) + " m" 
        },
        thresholdAlarm : function() { 
            let thrAlarm = parseFloat(this.device.metaData.thresholdAlarm)
            return thrAlarm .toFixed(2) + " m" 
        }
    },
    mounted() {
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "delta", "day"))
            .then(rec => this.levelDay = rec.data.body)
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "delta", "week"))
            .then(rec => this.levelWeek = rec.data.body)
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "delta", "month"))
            .then(rec => this.levelMonth = rec.data.body)
    }
}
</script>