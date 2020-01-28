<template>
    <v-container fluid>
        <v-row class="ml-2 mr-2">
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card name="fiume" 
                            :description="riverName"
                            icon="mdi-home"
                            color="success"
                            footerIcon="mdi-success"
                            footerDescription="stato : in allarme"
                            :footerColor="error"/>
            </v-col>
            
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Inalzamento medio"
                            description="1.50 m"
                            icon="mdi-swap-vertical-bold"
                            color="red"
                            footerIcon="mdi-history"
                            footerDescription="aggiornato : adesso" />
            </v-col>
                
            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Acqua caduta"
                            description="12.8 mm"
                            icon="mdi-water"
                            color="blue"
                            footerIcon="mdi-history"
                            footerDescription="aggiornato : adesso"/>
            </v-col>

            <v-col cols="12" :sm="6" :md="4" :lg="3">
                <stat-card  name="Umidità del terreno"
                            description="50 %"
                            icon="mdi-terrain"
                            color="brown"
                            footerIcon="mdi-history"
                            footerDescription="aggiornato : adesso" />
            </v-col>
        </v-row>
        <v-row>
            <p> mappa </p>
        </v-row>

        <v-row>
            <p> statistiche </p>
        </v-row>
    </v-container>
</template>

<script>
import StatCard from "@/components/StatsCard.vue"
export default {
    name : "river-page",
    props : {
        riverName : String
    },
    components : {
        "stat-card" : StatCard
    },
    computed : {
        alarm : function() {
            return false;
        }
    }
}
</script>