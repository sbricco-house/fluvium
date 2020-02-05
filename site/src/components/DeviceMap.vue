<template>
    <vl-map data-projection="EPSG:4326" :controls="true" :load-tiles-while-interacting="true" class="map" ref="map">
        <vl-view :center="center" :zoom="zoom" :enable-rotation="false" ref="mapView" ></vl-view>

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

        <vl-interaction-select :features.sync="selectedFeatures" :hit-tolerance="96"></vl-interaction-select>
        
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
        resolution: undefined,
        center: [12.493654,42.813583],
        zoom: 5
      }
    },
    watch: {
        devices(devices) {
            this.fitMapView(devices)
        },
        selectedFeatures(features) {
            if(features.length > 0) {
                this.onMarkerClick(features[0].properties);
            }
        }
    },
    filters: {
        mapLatLon: function(device) {
            if(!device) return [];
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
        },
        fitMapView(devices) {
            if(this.devices && this.devices.length > 0) {
                let validGpsDevices = this.devices.filter(dev => typeof(dev.data['gps']) !== 'undefined').map(dev => dev.data['gps']);
                let box = geo.calculateBoundingBox(validGpsDevices);
                let extent = geo.convertBounginBoxToExtent(box);
                this.$refs.mapView.fit(this.$refs.map.extentToViewProj(extent), {
                    size: this.$refs.map.$map.getSize(),
                    duration: 1000,
                    maxZoom: 16
                });
            }
        }
    }
}
</script>