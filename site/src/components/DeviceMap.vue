<template>
    <vl-map :controls="true" :load-tiles-while-interacting="true" class="map">
        <vl-view :zoom.sync="zoom" :center="center" :rotation.sync="rotation"></vl-view>

        <vl-layer-tile id="osm">
            <vl-source-osm attributions="fluvium powered by openstreemap"/>
        </vl-layer-tile>

        <template v-for="device in devices">
            <vl-feature v-if="device.data['gps']" :key="device.name" :properties="device">
                <vl-geom-point :coordinates="device | mapLatLon"></vl-geom-point>
                <vl-style-box>
                    <vl-style-icon v-if="isWaterlevelType(device) && alarm(device)" :src="require('@/assets/waves-alarm.png')" :scale="1" :size="[48, 48]"/>
                    <vl-style-icon v-else-if="isWaterlevelType(device)" :src="require('@/assets/waves.png')" :scale="1" :size="[48, 48]"/>
                    <vl-style-icon v-else :src="require('@/assets/terrain.png')" :scale="1" :size="[48, 48]"/>
                </vl-style-box>
            </vl-feature>
        </template>

        <vl-interaction-select :features.sync="selectedFeatures"></vl-interaction-select>
        
        </vl-map>
</template>


<style lang="scss" scoped>
.map {
  height: 400px;
}
</style>

<script>

import geo from '@/services/geo-utils.js'

export default {
    name : "device-map",
    props : {
        devices : Array
    },
    data () {
      return {
        selectedFeatures: [],
        zoom: 16,
        rotation: 0
      }
    },
    watch: {
        selectedFeatures(features) {
            if(features.length > 0) {
                this.onMarkerClick(features[0].properties);
            }
        }
    },
    filters: {
        mapLatLon: function(device) {
            if(!device  ) return [];
            return geo.convertLatLngToCoordinate(device.data.gps);
        }
    },
    methods: {
        onMarkerClick(device) {
            this.$emit('onDeviceSelected', device);
        },
        isWaterlevelType(device) {
            return device.name.split(':')[0] == "waterlevel";
        },
        alarm : function(device) {
            return device.metaData.alarm === "true";
        }
    },
    computed: {
        center: function() {
            if(this.devices && this.devices.length > 0) {
                let validGpsDevices = this.devices.filter(dev => typeof(dev.data['gps']) !== 'undefined').map(dev => dev.data['gps']);
                let boundingBox = geo.calculateBoundingBox(validGpsDevices);
                let coords = geo.convertLatLngToCoordinate(geo.centerOfBoundingBox(boundingBox));
                return coords;
            }
            return [0, 0];
        }
    }
}
</script>