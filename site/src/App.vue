<template>
  <v-app>
    <v-app-bar
      app
      color="primary"
      dark
    >
      <div class="d-flex align-center" @click="homepage">
        <v-img
          alt="Vuetify Logo"
          class="shrink mr-2 mb-1"
          contain
          src="@/assets/logo.png"
          transition="scale-transition"
          width="35"
        />
        
      </div>
      <v-toolbar-title class="display-1 pl-3">Fluvium</v-toolbar-title>
    </v-app-bar>
    <v-content>
      <router-view></router-view>
    </v-content>
    <v-footer padless>
    <v-col
      class="text-center"
      cols="12"
    >
      {{ new Date().getFullYear() }} — <strong>Andrea Petreti, Gianluca Aguzzi</strong>
    </v-col>
  </v-footer>
  </v-app>
</template>

<script>

export default {
  name: 'App',
  methods : {
    homepage : function() { this.$router.push("/") }
  }
};
</script>
