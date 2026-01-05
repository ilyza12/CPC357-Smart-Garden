import { defineStore } from 'pinia';
import { ref } from 'vue';
import { db } from '../services/firebaseConfig';
import { ref as dbRef, onValue, update } from 'firebase/database';

export const useGardenStore = defineStore('garden', () => {
  // 1. STATE
  const sensors = ref({ temperature: 0, humidity: 0, moisture: 0, rain: 0 });
  const controls = ref({ mode: 'MANUAL', pump_water: false, pump_fert: false });
  // NEW: Config State
  const config = ref({ moisture_threshold: 30, fert_time: '08:00' }); 

  // 2. LISTEN TO FIREBASE
  function initListener() {
    const gardenRef = dbRef(db, 'garden');
    
    onValue(gardenRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        if (data.sensors) sensors.value = data.sensors;
        if (data.controls) controls.value = data.controls;
        // NEW: specific check for config
        if (data.config) config.value = data.config; 
      }
    });
  }

  // 3. ACTIONS
  function togglePump(pump, status) {
    update(dbRef(db, 'garden/controls'), { [pump]: status });
  }

  function setMode(newMode) {
    update(dbRef(db, 'garden/controls'), { mode: newMode });
  }

  // NEW: Action to save thresholds
  function updateConfig(key, value) {
    update(dbRef(db, 'garden/config'), { [key]: value });
  }

  return { sensors, controls, config, initListener, togglePump, setMode, updateConfig };
});