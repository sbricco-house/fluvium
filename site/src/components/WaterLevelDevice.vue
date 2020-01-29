<template>
    <v-container fluid>
        <v-row class="ml-2 mr-2">
            <v-col cols="12" :sm="6" :md="6" :lg="3">
                <stat-card name="Stato" 
                    :description="state"
                    icon="mdi-information"
                    color="info" />
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
                            color="warning" />
                
            </v-col>
        </v-row>
        <v-row class="ml-2 mr-2 mt-10">
            <v-col cols="12" :md="12" :lg="4">
                <data-chart color="#00BCD4" title="Livello acqua" footerHistory="Ultimo giorno" 
                            icon="mdi-swap-vertical-bold"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="4">
                <data-chart color="#00BCD4" title="Livello acqua" footerHistory="Ultimi 7 giorni" 
                            icon="mdi-swap-vertical-bold"/>
            </v-col>
            <v-col cols="12" :md="12" :lg="4">
                 <data-chart color="#00BCD4" title="Livello acqua" footerHistory="Ultimo mese" 
                            icon="mdi-swap-vertical-bold"/>
            </v-col>
        </v-row>
    </v-container>
</template>

<script>

import StatCard from "@/components/StatsCard.vue"
import DataChart from "@/components/DataChart.vue"
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
            let dateSensed = new Date(this.device.data.water_level.timestamp)
            return "aggiornato : " + dateSensed.toLocaleString('it-IT', options);
        },
        state : function() {
            if(this.device.metaData.alarm === "true") {
                return "In allarme"
            } else {
                return "Nella norma"
            }
        },
        waterLevel : function() {
            return this.device.data.water_level.delta + " m"
        },
        thresholdAlarm : function() {
            return this.device.metaData.thresholdAlarm + " m"
        }
    }
}
</script>