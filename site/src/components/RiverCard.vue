<template>
    <v-card v-ripple class="elevation-10" :to="riverPagePath">
        <v-img
            :src="river.imageSrc"
            class="white--text align-end"
            height="200px"
            gradient="rgba(100,115,201,0), rgba(15,12,12,.8)"
        >
            <v-card-title class="display-1">{{river.river | capitalize}}</v-card-title>
        </v-img>
        
    </v-card>
</template>

<script>
export default {
    name : "river",
    props : {
        river : Object
    },
    computed : {
        riverPagePath : function() {
            return "/river/" + this.river.river
        }
    }
}
</script>