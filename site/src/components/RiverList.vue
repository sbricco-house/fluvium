<template>
    <v-container fluid>
        <v-row class="mt-n2">
            <v-col
                v-for="river in rivers"
                :key="river.name"
                cols="12"
                lg="4"
                md="6"
                xs="12"
            >
                <river-card :river="river"/>
            </v-col>
        </v-row>
    </v-container>
</template>

<script>

import RiverCard from "@/components/RiverCard.vue"
export default {
  name: 'river-list',
  components : {
      'river-card' : RiverCard
  },
  props : {
    rivers : Array
  }
}
</script>