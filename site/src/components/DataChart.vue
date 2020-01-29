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
export default {
    components: {
        apexchart: VueApexCharts,
    },
    props : {
        color : String,
        title : String,
        footerHistory : String,
        icon : String,
    },
    computed: {
        
        sheetColor : function() {
            return "background-color: " + this.color
        },
        series : function() {
            return [{
                name: this.title,
                data: [10, 41, 35, 51, 49, 62, 69, 91, 148],
            }]
        }
    },
    data: () => ({
        
        chartOptions: {
            grid: {
                show: false
            },
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
                categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep'],
                labels: {
                    style: {
                        colors: "#fff",
                        fontSize: '15px',
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
                labels: {
                    style: {
                        color : "#fff",
                        fontSize: '15px',
                    },
                }
            }
        },
    }),
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