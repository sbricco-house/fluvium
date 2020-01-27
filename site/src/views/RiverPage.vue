<template>
    <v-container fluid>
        <v-row>
            <p> fiume {{riverName}}: stato : </p>
            <stat-card/>
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
    }
}
</script>