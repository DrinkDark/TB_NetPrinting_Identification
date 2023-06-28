
import React from 'react'
import { PermissionsAndroid} from "react-native";
import { BleManager, Device } from "react-native-ble-plx";
import base64 from 'react-native-base64'
type PermissionCallback = (result: boolean) => void;

const bleManager = new BleManager();


const cardIdUUIDCharac = '495f449c-fc60-4048-b53e-bdb3046d4495';
const cardIdUUIDService = '5a44c004-4112-4274-880e-cd9b3daedf8e';

interface BluetoothlowEnergyApi {
    requestPermissions(callback: PermissionCallback): Promise<void>;
    scanForDevices(): void;
}

function useBle(): BluetoothlowEnergyApi{

    const requestPermissions = async (callback: PermissionCallback) => {
        const grantedStatus = await PermissionsAndroid.request(
            PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
            {
                title: 'Location Permission',
                message: 'Bluetooth Low Energy Needs Location Permission',
                buttonNegative: 'Cancel',
                buttonPositive: 'Ok',
                buttonNeutral: "Maybe Later"
            },
        );
        await PermissionsAndroid.requestMultiple([ PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN, PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT])
        callback(grantedStatus === PermissionsAndroid.RESULTS.GRANTED);
    }

    const isDuplicteDevice = (devices: Device[], nextDevice: Device) =>
    devices.findIndex(device => nextDevice.id === device.id) > -1;

    const scanForDevices = () =>
        console.log("Start scanning...");
        bleManager.startDeviceScan(null, null, (error, device) => {
            if(error) {
                console.log('Error scanning for devices : ', error);
            } 
            if (device && device.name == "TWN4 BLE"){
                console.log('Device found : ',device.name);
                console.log('Stop scanning.');
                connectToDevice(device);  
            }
    });

    const connectToDevice = async (device: Device) => {
        try {
            bleManager.stopDeviceScan();
            const deviceConnection = await bleManager.connectToDevice(device.id);
            await deviceConnection.discoverAllServicesAndCharacteristics();
            console.log('Device connected : ', device.name);
            const valueSend = await bleManager.writeCharacteristicWithResponseForDevice(device.id, cardIdUUIDService, cardIdUUIDCharac, base64.encode('041350322c46680'));
            console.log('Value send : 041350322c46680');
        } catch (e) {
            console.log('FAILED TO CONNECT', e);
        }

        
    };

    const sendValue = async (device: Device) => {
        try {
            const valueSend = await bleManager.writeCharacteristicWithoutResponseForDevice(device.id, cardIdUUIDService, cardIdUUIDCharac, base64.encode('041350322c46680'));
        } catch (e) {
            console.log('FAILED TO CONNECT', e);
        }
    };

    const disconnectFromDevice = () => {
       /* if (connectedDevice) {
          bleManager.cancelDeviceConnection(connectedDevice.id);
          setConnectedDevice(null);
        }*/
      };

    return {
        requestPermissions,
        scanForDevices
    } ;
}

function requestMultiple(arg0: any[]): any {
    throw new Error("Function not implemented.");
}

export default useBle;