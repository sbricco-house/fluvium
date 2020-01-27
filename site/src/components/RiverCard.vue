<template>
    <v-card v-ripple class="elevation-10" :to="riverPagePath">
        <v-img
            :src="river.src"
            class="white--text align-end"
            height="200px"
        >
            <v-card-title class="display-1" v-text="river.name"></v-card-title>
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
            return "/river/" + this.river.name
        }
    }
}
</script>