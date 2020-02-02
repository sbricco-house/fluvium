<template>
    <v-card class="mt-4 " elevation="6" height="350">
        <div id="chart" class="elevation-3 ml-4 mr-4 v-sheet--offset">
            <apexchart type="line" :style="sheetColor" height="250" :options="chartOptions" :series="series" ></apexchart>
        </div>
        <v-card-text class="pt-0 text-left">
          <div class="headline font-weight-light "><v-icon class="mr-2">{{icon}}</v-icon>{{title}} </div>
        </v-card-text>
        <v-divider class="ml-2 mr-2"></v-divider>
        <div class="pl-2 pb-1 pt-1">
            <v-icon size=18 >mdi-history</v-icon> <span class="ml-2 overline" >{{footerHistory}}</span>
        </div>
    </v-card>
</template>

<script>
import VueApexCharts from 'vue-apexcharts'
let DateUtils = require("@/dateUtils.js");

export default {
    components: {
        apexchart: VueApexCharts,
    },
    props : {
        color : String,
        title : String,
        footerHistory : String,
        dataFormat : Object,
        icon : String,
        data : Array
    },
    computed: {
        xShow : function() {
            let firstDate = Object.assign({}, this.dataFormat)
            let secondDate = Object.assign({}, DateUtils.monthAndDay)
            return JSON.stringify(firstDate) === JSON.stringify(secondDate) ? false : true
        },
        sensed : function() {
            return this.data.map(element => (element.data.toFixed(2)))
        },
        timestamps : function() {
            return this.data.map(element => new Date(element.timestamp).toLocaleString('it-IT', this.dataFormat))
        },
        sheetColor : function() {
            return "background-color: " + this.color
        },
        series : function() {
            return [{
                name: this.title,
                data: this.sensed,
            }]
        },
        chartOptions : function() {
            return {
                    grid: {
                    show: false
                },
                responsive: [{
                    breakpoint: 800,
                    options: {
                        xaxis: {                
                            labels: {
                                show : false
                            }
                        }
                    }
                }],
                chart: {
                    type: 'line',
                    zoom: {
                        enabled: false 
                    },
                    toolbar: {
                        show: false
                    },
                },
                stroke: {
                    curve: 'smooth',
                    width: 5,
                    colors : ["#fff"]
                },
                
                xaxis: {
                    tickAmount: 1,
                    categories: this.timestamps,            
                    
                    labels: {
                        show : true,
                        style: {
                            colors: "#fff",
                            fontSize: '12px',
                        },
                    },
                    axisBorder: {
                        show: false,
                    },
                    axisTicks: {
                        show: false,
                    },
                },
                yaxis: {
                    min : Math.min(...this.sensed) - 1,
                    max : Math.max(...this.sensed) + 1,
                    labels: {
                        style: {
                            color : "#fff",
                            fontSize: '12px',
                        },
                    }
                }
            }
        }
    }
}
</script>

<style scoped>
  .v-sheet--offset {
    top: -14px;
    position: relative;
  }
  .white {
      color:white
  }
</style>