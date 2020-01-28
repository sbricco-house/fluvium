<template>
  <v-card
    class="mt-4"
    elevation="6"
  >
    <v-row>
      <v-col cols="3" class="text-center justify-center align-center">
        
        <v-sheet
          class="v-sheet--offset ml-2 sm"
          :color="color"
          elevation="12"
          height="80px"
          width="80px"
        >
          <v-icon color="white" class="mt-1" size=70>{{icon}}</v-icon>
        </v-sheet>
      </v-col>
      <v-col cols="9">
        <v-card-text class="pt-0 text-right">
          <div class="subheading font-weight-black mb-2 grey--text" >{{name}}</div>
          <div class="headline font-weight-light ">{{description}}</div>
          </v-card-text>
      </v-col>
    </v-row>
    <v-divider></v-divider>
    <div class="pl-2 pb-1 pt-1">
      <v-icon size=18 :color="footerColor">{{footerIcon}}</v-icon> <span class="ml-2 overline" >{{footerDescription}}</span>
      
    </div>
  </v-card>
</template>
<script>
export default {
    name : "stat-card",
    props : {
      name : String,
      description : String,
      icon: String,
      color : String,
      footerIcon: String,
      footerDescription : String,
      footerColor: String,
    }
}
</script>

<style scoped>
  .v-sheet--offset {
    top: -24px;
    position: relative;
  }
</style>