// frontend/src/services/firebaseConfig.js
import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyC69c50BFvNrD5MmAwYJKNO9VaZ_hhRJso",
  authDomain: "smartgarden-22f5f.firebaseapp.com",
  databaseURL: "https://smartgarden-22f5f-default-rtdb.firebaseio.com",
  projectId: "smartgarden-22f5f",
  storageBucket: "smartgarden-22f5f.firebasestorage.app",
  messagingSenderId: "19721938972",
  appId: "1:19721938972:web:c01165a46d836e2fa217d9",
  measurementId: "G-MQ13DVC36S"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

export { db };