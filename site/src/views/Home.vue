<template>
  <div class="home">
    <river-list :rivers="rivers"/>
  </div>
</template>

<script>
import RiverList from '@/components/RiverList.vue'
import aws from "@/services/aws-lambda.js"
export default {
  name: 'home',
  components: {
    'river-list' : RiverList
  },
  data () {
      return {
        rivers : [],
      }
  },
  mounted() {
    aws.executeLambda("GetRivers").then(result => this.rivers = result.data.body)
  }
}
</script>
