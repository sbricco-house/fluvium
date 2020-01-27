<template>
  <v-card
    class="mt-4"
    max-width="400"
  >
    <v-row>
      <v-col cols="3">
        
        <v-sheet
          class="v-sheet--offset ml-2 sm"
          color="cyan"
          elevation="12"
          height="70px"
          width="70px"
        >
        
          <v-icon>mdi-home</v-icon>
        </v-sheet>
      </v-col>
      <v-col cols="9">
        <v-card-text class="pt-0">
          <div class="title font-weight-light mb-2">User Registrations</div>
          <div class="subheading font-weight-light grey--text">Last Campaign Performance</div>
          </v-card-text>
      </v-col>
    </v-row>

    
  </v-card>
</template>
<script>
export default {
    name : "stat-card"
}
</script>

<style scoped>
  .v-sheet--offset {
    top: -24px;
    position: relative;
  }
</style>