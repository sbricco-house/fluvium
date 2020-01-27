<template>
  <div class="home">
    <river-list :rivers="rivers"/>
  </div>
</template>

<script>
// @ is an alias to /src
import RiverList from '@/components/RiverList.vue'

export default {
  name: 'home',
  components: {
    'river-list' : RiverList
  },
  data () {
      return {
        rivers : [
          {
            name: "Savio",
            src: "https://upload.wikimedia.org/wikipedia/commons/f/f5/Camminata_lungo_il_fiume.jpg"
          },
          {
            name: "Cesano",
            src: "https://upload.wikimedia.org/wikipedia/commons/0/0c/Fiume_Cesano.jpg"
          },
          {
            name: "Arno",
            src: "https://www.firenze-online.com/img/conoscere/foto-fiumearno2.jpg"
          },
          {
            name: "Po",
            src: "https://www.skuola.net/news_foto/2018/affluenti-po.jpg"
          }
        ]
      }
  }
}
</script>
