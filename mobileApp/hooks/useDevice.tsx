import { useState, useEffect } from 'react';
import axios from 'axios';
import { Device } from 'react-native-ble-plx';
const ipAddress = '10.93.11.8';

const useDevice = () => {
  const [connectedDevice, setConnectedDevice] = useState<Device | null>(null);
  const [printedText, setPrintedText] = useState('');

  useEffect(() => {

  }, []);

  return [connectedDevice, setConnectedDevice, printedText, setPrintedText];
};

export default useDevice;


