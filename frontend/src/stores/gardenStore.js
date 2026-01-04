// frontend/src/stores/gardenStore.js
import { defineStore } from 'pinia';
import { ref } from 'vue';
import { db } from '../services/firebaseConfig';
import { ref as firebaseRef, onValue, set } from "firebase/database";

export const useGardenStore = defineStore('garden', () => {
  // --- STATE ---
  const sensors = ref({
    temperature: 0,
    humidity: 0,
    moisture: 0,
    rain: 0 
  });

  const controls = ref({
    pump_water: false,
    pump_fert: false,
    mode: 'MANUAL' // 'AUTO' or 'MANUAL'
  });

  const isConnected = ref(false);

  // --- ACTIONS ---
  
  // 1. Listen to Firebase (Run this when app starts)
  function initListener() {
    const gardenRef = firebaseRef(db, 'garden/');
    
    onValue(gardenRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        isConnected.value = true;
        if (data.sensors) sensors.value = { ...sensors.value, ...data.sensors };
        if (data.controls) controls.value = { ...controls.value, ...data.controls };
      }
    });
  }

  // 2. Control Pumps
  async function togglePump(pumpType, status) {
    try {
      await set(firebaseRef(db, `garden/controls/${pumpType}`), status);
    } catch (e) {
      console.error(e);
    }
  }

  // 3. Set Mode
  async function setMode(mode) {
    try {
      await set(firebaseRef(db, 'garden/controls/mode'), mode);
    } catch (e) {
      console.error(e);
    }
  }

  return { sensors, controls, isConnected, initListener, togglePump, setMode };
});