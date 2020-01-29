import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import vuetify from './plugins/vuetify';
import VueLayers from 'vuelayers'
import './aws-config';

import 'vuelayers/lib/style.css' // needs css-loader

Vue.use(VueLayers, {
  dataProjection: 'EPSG:4326',
});

Vue.config.productionTip = false

Vue.filter('capitalize', function (value) {
  if (!value) return value
  value = value.toString()
  return value.charAt(0).toUpperCase() + value.slice(1)
})
new Vue({
  router,
  store,
  vuetify,
  render: h => h(App)
}).$mount('#app')
