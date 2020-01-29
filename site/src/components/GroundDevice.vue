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
            <v-col cols="12" :md="12" :lg="4">
                <data-chart color="#795548" title="Umidità" footerHistory="Ultimo giorno" icon="mdi-terrain"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="4">
                <data-chart color="#795548" title="Umidità" footerHistory="Ultimi 7 giorni" icon="mdi-terrain"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="4">
                 <data-chart color="#795548" title="Umidità" footerHistory="Ultimo mese" icon="mdi-terrain"/>
            </v-col>
        </v-row>
        <v-row class="ml-2 mr-2">
            <v-col cols="12" :md="12" :lg="4">
                <data-chart color="#2196F3" title="Acqua caduta" footerHistory="Ultimo giorno" icon = "mdi-water"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="4">
                <data-chart color="#2196F3" title="Acqua caduta" footerHistory="Ultimi 7 giorni" icon = "mdi-water"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="4">
                 <data-chart color="#2196F3" title="Acqua caduta" footerHistory="Ultimo mese" icon = "mdi-water"/>
            </v-col>
        </v-row>
    </v-container>
</template>

<script>

import DataChart from "@/components/DataChart.vue"
import StatCard from "@/components/StatsCard.vue"
export default {
    name : "water-level-device",
    components : {
        "stat-card" : StatCard,
        'data-chart' : DataChart
    },
    props : {
        device : Object
    },
    computed: {
        updatedAt : function() {
            var options = {
                'month': '2-digit', 
                'day': '2-digit', 
                'year' : '2-digit', 
                'hour' : 'numeric',
                'minute' : 'numeric'
            };
            let dateSensed = new Date(this.device.data.ground.timestamp)
            return "aggiornato : " + dateSensed.toLocaleString('it-IT', options);
        },
        soilMoisture : function() {
            console.log(this.device.data.ground)
            return this.device.data.ground.soil_moisture.toFixed(2) + " %"
        },
        rainQuantity : function() {
            return this.device.data.ground.rain_quantity.toFixed(2) + " mm"
        }
    }
}
</script>