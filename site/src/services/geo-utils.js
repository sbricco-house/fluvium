
const MAX_LAT = 90;
const MAX_LONG = 180;
const MIN_LAT = -MAX_LAT;
const MIN_LONG = -MAX_LONG;

export default {
    convertLatLngToCoordinate: function(latLng) {
        return [parseFloat(latLng.long), parseFloat(latLng.lat)];
    },
    calculateBoundingBox: function(latLongs) {
        let west = MAX_LONG;
        let east = MIN_LONG;
        let north = MIN_LAT;
        let south = MAX_LAT;
    
        latLongs.forEach(loc => {
            north = Math.max(loc.lat, north);
            south = Math.min(loc.lat, south);
    
            east = Math.max(loc.long, east);
            west = Math.min(loc.long, west);
        });
    
        return { topRight: { lat: north, long: west }, bottomLeft: { lat: south, long: east } };
    },
    centerOfBoundingBox: function(boundingBox) {
        return {
            lat: (boundingBox.topRight.lat + boundingBox.bottomLeft.lat) / 2,
            long: (boundingBox.topRight.long + boundingBox.bottomLeft.long) / 2
        }
    }
}
