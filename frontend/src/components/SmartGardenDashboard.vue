<template>
  <div class="dashboard-container">
    <header class="header">
      <div class="logo-section">
        <SparklesIcon class="leaf-icon" />
        <div>
          <h1>Smart Garden Monitor</h1>
          <p>Real-time sensor monitoring and intelligent irrigation control</p>
        </div>
      </div>
    </header>

    <div class="card control-mode-card">
      <div class="card-header-row">
        <h3>Control Mode</h3>
        <div class="toggle-container">
          <span :class="{ active: !isAutoMode }">Manual Mode</span>
          <label class="switch">
            <input type="checkbox" v-model="isAutoMode">
            <span class="slider round"></span>
          </label>
          <span :class="{ active: isAutoMode }">Automatic Mode</span>
        </div>
      </div>
      <p class="mode-desc">
        {{ isAutoMode ? 'Pumps activate automatically based on sensor thresholds' : 'Control pumps manually with buttons' }}
      </p>
    </div>

    <div class="grid-4">
      <div class="card sensor-card">
        <div class="card-header">
          <span>Temperature</span>
          <component :is="BeakerIcon" class="icon-sm text-red-500" />
        </div>
        <div class="value green-text">{{ store.sensors.temperature }}°C</div>
      </div>

      <div class="card sensor-card">
        <div class="card-header">
          <span>Humidity</span>
          <component :is="CloudIcon" class="icon-sm text-blue-500" />
        </div>
        <div class="value green-text">{{ store.sensors.humidity }}%</div>
      </div>

      <div class="card sensor-card">
        <div class="card-header">
          <span>Soil Moisture</span>
          <component :is="SunIcon" class="icon-sm text-green-500" />
        </div>
        <div class="value green-text">{{ store.sensors.moisture }}%</div>
      </div>

      <div class="card sensor-card">
        <div class="card-header">
          <span>Rain Intensity</span>
          <component :is="BoltIcon" class="icon-sm text-yellow-500" />
        </div>
        <div class="value green-text">{{ store.sensors.rain }}%</div>
      </div>
    </div>

    <div class="grid-2">
      <div class="card chart-card">
        <h3>Temperature History</h3>
        <div class="chart-container">
          <Line :data="tempChartData" :options="chartOptions" />
        </div>
      </div>
      <div class="card chart-card">
        <h3>Soil Moisture History</h3>
        <div class="chart-container">
          <Line :data="soilChartData" :options="chartOptions" />
        </div>
      </div>
      <div class="card chart-card">
        <h3>Humidity History</h3>
        <div class="chart-container">
          <Line :data="humidityChartData" :options="chartOptions" />
        </div>
      </div>
      <div class="card chart-card">
        <h3>Rain Intensity History</h3>
        <div class="chart-container">
          <Line :data="rainChartData" :options="chartOptions" />
        </div>
      </div>
    </div>

    <div class="grid-2 bottom-section">
      
      <div class="card pump-card">
        <div class="card-header">
          <h3>⚡ Pump Control</h3>
        </div>
        
        <div v-if="!isAutoMode" class="pump-controls">
          <div class="pump-row">
            <span>💧 Watering Pump</span>
            <span class="badge" :style="{ background: store.controls.pump_water ? '#86efac' : '#f1f5f9' }">
              {{ store.controls.pump_water ? 'Active' : 'Idle' }}
            </span>
          </div>
          <button 
            class="btn btn-primary" 
            @click="togglePump('water')"
            :style="{ opacity: store.controls.pump_water ? '0.7' : '1' }"
          >
            {{ store.controls.pump_water ? 'Stop Watering' : 'Start Watering' }}
          </button>
          
          <div class="pump-row mt-4">
            <span>🧪 Fertilizer Pump</span>
            <span class="badge" :style="{ background: store.controls.pump_fert ? '#86efac' : '#f1f5f9' }">
              {{ store.controls.pump_fert ? 'Active' : 'Idle' }}
            </span>
          </div>
          <button 
            class="btn btn-primary" 
            @click="togglePump('fertilizer')"
            :style="{ opacity: store.controls.pump_fert ? '0.7' : '1' }"
          >
            {{ store.controls.pump_fert ? 'Stop Fertilizing' : 'Start Fertilizing' }}
          </button>
        </div>

        <div v-else class="auto-message">
          <p class="status-ok">System is running in Auto-Pilot.</p>
          <p class="sub-text">Pumps will trigger if sensors go below threshold.</p>
        </div>
      </div>

      <div v-if="isAutoMode" class="card threshold-card">
        <h3>⚙️ Automation Thresholds</h3>
        
        <div class="input-group">
          <label>Soil Moisture Threshold (%)</label>
          <input 
            type="number" 
            :value="store.config.moisture_threshold" 
            @change="updateMoisture"
            placeholder="30"
          >
          <p class="hint">Watering activates when soil moisture falls below this level</p>
        </div>

        <div class="input-group">
          <label>Daily Fertilization Time</label>
          <input 
            type="time" 
            :value="store.config.fert_time" 
            @change="updateTime"
          >
          <p class="hint">Fertilizer pump activates automatically at this time</p>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { computed, onMounted } from 'vue';
import { useGardenStore } from '../stores/gardenStore'; // Correct Store Import

// 1. Import Icons
import { 
  BeakerIcon, CloudIcon, BoltIcon, SunIcon, SparklesIcon 
} from '@heroicons/vue/24/outline';

// 2. Import Chart.js
import {
  Chart as ChartJS, CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend
} from 'chart.js';
import { Line } from 'vue-chartjs';

// 3. Register ChartJS
ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

// --- CONNECT TO STORE ---
const store = useGardenStore();

// Start listening to Firebase when page loads
onMounted(() => {
  store.initListener();
});

// --- COMPUTED PROPERTIES ---
// Convert Firebase String ('AUTO'/'MANUAL') to Boolean for Switch
const isAutoMode = computed({
  get: () => store.controls.mode === 'AUTO',
  set: (val) => store.setMode(val ? 'AUTO' : 'MANUAL')
});

// --- UPDATED ACTIONS FOR CONFIG ---
// Save Moisture Threshold to Firebase
const updateMoisture = (e) => {
  const newVal = Number(e.target.value);
  store.updateConfig('moisture_threshold', newVal);
};

// Save Time Schedule to Firebase
const updateTime = (e) => {
  const newVal = e.target.value; // e.g., "08:00"
  store.updateConfig('fert_time', newVal);
};

// --- PUMP ACTIONS ---
const togglePump = (type) => {
  const pumpKey = type === 'water' ? 'pump_water' : 'pump_fert';
  const currentState = store.controls[pumpKey];
  store.togglePump(pumpKey, !currentState);
};

// --- DUMMY CHART DATA (Keep for now) ---
const chartOptions = { responsive: true, maintainAspectRatio: false, plugins: { legend: { display: false } }, scales: { y: { beginAtZero: true } } };
const tempChartData = { labels: ['10:00', '10:05', '10:10', '10:15', '10:20', '10:25'], datasets: [{ label: 'Temperature', backgroundColor: '#ef4444', borderColor: '#ef4444', data: [24, 25, 25, 26, 26, 25], tension: 0.4 }] };
const soilChartData = { labels: ['10:00', '10:05', '10:10', '10:15', '10:20', '10:25'], datasets: [{ label: 'Soil Moisture', backgroundColor: '#3b82f6', borderColor: '#3b82f6', data: [60, 58, 55, 53, 50, 48], tension: 0.4 }] };
const humidityChartData = { labels: ['10:00', '10:05', '10:10', '10:15', '10:20', '10:25'], datasets: [{ label: 'Humidity', backgroundColor: '#8b5cf6', borderColor: '#8b5cf6', data: [65, 66, 68, 67, 65, 64], tension: 0.4 }] };
const rainChartData = { labels: ['10:00', '10:05', '10:10', '10:15', '10:20', '10:25'], datasets: [{ label: 'Rain Intensity', backgroundColor: '#0ea5e9', borderColor: '#0ea5e9', data: [0, 0, 10, 45, 80, 20], tension: 0.4 }] };
</script>

<style scoped>
/* Base Layout */
.dashboard-container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  color: #2c3e50;
  background: linear-gradient(180deg, #86efac 0%, #f0fdf4 100%);
  min-height: 100vh;
} 

/* Header */
.header { text-align: center; margin-bottom: 30px; }
.logo-section { display: flex; align-items: center; justify-content: center; gap: 10px; color: #15803d; }
.leaf-icon { width: 40px; height: 40px; }
h1 { color: #000; margin: 0; font-size: 1.8rem; }
p { color: #666; font-size: 0.9rem; }

/* Cards */
.card {
  background: white;
  border-radius: 12px;
  padding: 20px;
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
  border: 1px solid #eef2f6;
}

/* Grid Layouts */
.grid-4 { display: grid; grid-template-columns: repeat(4, 1fr); gap: 20px; margin-bottom: 20px; }
.grid-2 { display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; margin-bottom: 20px; }

/* Sensor Cards */
.sensor-card .value { font-size: 2rem; font-weight: bold; margin-top: 10px; }
.green-text { color: #22c55e; }
.card-header { display: flex; justify-content: space-between; align-items: center; font-weight: 600; color: #000000; }
.icon-sm { width: 24px; height: 24px; }

/* Chart Container */
.chart-container { height: 200px; width: 100%; }

/* Toggle Switch */
.control-mode-card { margin-bottom: 20px; display: flex; flex-direction: column; align-items: center; }
.card-header-row { display: flex; align-items: center; justify-content: space-between; width: 100%; margin-bottom: 10px; }
.toggle-container { display: flex; align-items: center; gap: 15px; }
.switch { position: relative; display: inline-block; width: 50px; height: 24px; }
.switch input { opacity: 0; width: 0; height: 0; }
.slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }
.slider:before { position: absolute; content: ""; height: 16px; width: 16px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
input:checked + .slider { background-color: #22c55e; }
input:checked + .slider:before { transform: translateX(26px); }
.active { font-weight: bold; color: #000000; }

/* Buttons & Inputs */
.btn-primary { background-color: #0f172a; color: white; width: 100%; padding: 10px; border-radius: 6px; border: none; cursor: pointer; margin-top: 5px; transition: background 0.3s; }
.btn-primary:hover { background-color: #1e293b; }

.input-group { margin-bottom: 15px; }
.input-group input { width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 6px; background: #f8fafc; box-sizing: border-box; }
.hint { font-size: 0.8rem; color: #888; margin-top: 4px; }
.badge { background: #f1f5f9; padding: 2px 8px; border-radius: 4px; font-size: 0.8rem; }
.pump-row { display: flex; justify-content: space-between; align-items: center; }
.mt-4 { margin-top: 1rem; }
.auto-message { text-align: center; color: #666; }
.status-ok { color: #15803d; font-weight: bold; }
.sub-text { font-size: 0.85rem; color: #888; margin-top: 4px; }

/* Responsive */
@media (max-width: 768px) {
  .grid-4, .grid-2 { grid-template-columns: 1fr; }
  .toggle-container { font-size: 0.8rem; }
}
</style>