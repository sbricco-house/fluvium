<template>
    <v-container fluid>
        <v-row class="ml-2 mr-2">
            <v-col cols="12" :sm="6" :md="6" :lg="3">
                <stat-card name="Umitidà" 
                            :description="soilMoisture"
                            icon="mdi-terrain"
                            color="brown"
                            footerIcon="mdi-history"
                            :footerDescription="updatedAt" />
                
            </v-col>
            <v-col cols="12" :sm="6" :md="6" :lg="3">
                <stat-card name="Acqua caduta" 
                            :description="rainQuantity"
                            icon="mdi-water"
                            color="blue"
                            footerIcon="mdi-history"
                            :footerDescription="updatedAt" />
            </v-col>
        </v-row>
        <v-row class="ml-2 mr-2 mt-10">
            <v-col cols="12" :md="12" :lg="6">
                <data-chart color="#795548" 
                            title="Umidità" 
                            footerHistory="Ultimo giorno" 
                            icon="mdi-terrain"
                            :dataFormat="formats.hour"
                            :data="soilDay"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="6">
                <data-chart color="#795548" 
                            title="Umidità" 
                            footerHistory="Ultimi 7 giorni" 
                            icon="mdi-terrain"
                            :dataFormat="formats.day"
                            :data="soilWeek"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="12">
                 <data-chart color="#795548" 
                            title="Umidità" 
                            footerHistory="Ultimo mese" 
                            icon="mdi-terrain"
                            :dataFormat="formats.monthAndDay"
                            :data="soilMonth"/>
            </v-col>
        </v-row>
        <v-row class="ml-2 mr-2">
            <v-col cols="12" :md="12" :lg="6">
                <data-chart color="#2196F3" 
                            title="Acqua caduta" 
                            footerHistory="Ultimo giorno" 
                            icon = "mdi-water"
                            :dataFormat="formats.hour"
                            :data="rainDay"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="6">
                <data-chart color="#2196F3" 
                            title="Acqua caduta" 
                            footerHistory="Ultimi 7 giorni" 
                            icon = "mdi-water"
                            :dataFormat="formats.day"
                            :data="rainWeek"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="12">
                 <data-chart color="#2196F3" 
                            title="Acqua caduta" 
                            footerHistory="Ultimo mese" 
                            icon = "mdi-water"
                            :dataFormat="formats.monthAndDay"
                            :data="rainMonth"/>
            </v-col>
        </v-row>
    </v-container>
</template>

<script>

import DataChart from "@/components/DataChart.vue"
import StatCard from "@/components/StatsCard.vue"
import DateUtils from "@/dateUtils.js"
import aws from "@/services/aws-lambda.js"
let utils = require("@/awsUtils");

export default {
    name : "water-level-device",
    components : {
        "stat-card" : StatCard,
        'data-chart' : DataChart
    },
    props : {
        device : Object
    },
    data() {
        return {
            formats : DateUtils,    
            soilDay : [],
            soilWeek : [],
            soilMonth : [],
            rainDay : [],
            rainWeek : [],
            rainMonth : [],
        }
    },
    computed: {
        updatedAt : function() {
            let dateSensed = new Date(this.device.data.ground.timestamp)
            return "aggiornato : " + dateSensed.toLocaleString('it-IT', DateUtils.fullDate);
        },
        soilMoisture : function() {
            console.log(this.device.data.ground)
            return this.device.data.ground.soil_moisture.toFixed(2) + " %"
        },
        rainQuantity : function() {
            return this.device.data.ground.rain_quantity.toFixed(2) + " mm"
        }
    },
    mounted() {
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "soil_moisture", "day"))
            .then(rec => this.soilDay = rec.data.body)
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "soil_moisture", "week"))
            .then(rec => this.soilWeek = rec.data.body)
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "soil_moisture", "month"))
            .then(rec => this.soilMonth = rec.data.body)

        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "rain_quantity", "day"))
            .then(rec => this.rainDay = rec.data.body)
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "rain_quantity", "week"))
            .then(rec => this.rainWeek = rec.data.body)
        aws.executeLambda("GetSensorStats", utils.createStatRequest(this.device.name, "rain_quantity", "month"))
            .then(rec => this.rainMonth = rec.data.body)
    }
}
</script>