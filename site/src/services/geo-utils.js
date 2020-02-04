
const MAX_LAT = 90;
const MAX_LONG = 180;
const MIN_LAT = -MAX_LAT;
const MIN_LONG = -MAX_LONG;

const LATITUDE_KEY = 'latitude'
const LONGITUDE_KEY = 'longitude'

export default {
    convertLatLngToCoordinate: function(latLng) {
        return [parseFloat(latLng[LONGITUDE_KEY]), parseFloat(latLng[LATITUDE_KEY])];
    },
    calculateBoundingBox: function(latLongs) {
        let west = MAX_LONG;
        let east = MIN_LONG;
        let north = MIN_LAT;
        let south = MAX_LAT;
    
        latLongs.forEach(loc => {
            north = Math.max(loc[LATITUDE_KEY], north);
            south = Math.min(loc[LATITUDE_KEY], south);
    
            east = Math.max(loc[LONGITUDE_KEY], east);
            west = Math.min(loc[LONGITUDE_KEY], west);
        });
    
        return { 
            topRight: { [LATITUDE_KEY]: north, [LONGITUDE_KEY]: west }, 
            bottomLeft: { [LATITUDE_KEY]: south, [LONGITUDE_KEY]: east } 
        };
    },
    centerOfBoundingBox: function(boundingBox) {
        return {
            [LATITUDE_KEY]: (boundingBox.topRight[LATITUDE_KEY] + boundingBox.bottomLeft[LATITUDE_KEY]) / 2,
            [LONGITUDE_KEY]: (boundingBox.topRight[LONGITUDE_KEY] + boundingBox.bottomLeft[LONGITUDE_KEY]) / 2
        }
    }
}
