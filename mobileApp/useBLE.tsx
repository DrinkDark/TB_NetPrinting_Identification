
import React from 'react'
import { Alert, PermissionsAndroid, Pressable, StyleSheet, Text, View} from "react-native";
import { BleManager, Device } from "react-native-ble-plx";
import base64 from 'react-native-base64'
type PermissionCallback = (result: boolean) => void;

const bleManager = new BleManager();


const cardIdUUIDCharac = '495f449c-fc60-4048-b53e-bdb3046d4495';
const cardIdUUIDService = '5a44c004-4112-4274-880e-cd9b3daedf8e';

interface BluetoothlowEnergyApi {
    requestPermissions(callback: PermissionCallback): Promise<void>;
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

    const scanForDevices = (userID : String) => {
        if(userID !== ''){
            console.log("Start scanning...");
            bleManager.startDeviceScan(null, null, (error, device) => {
                if(error) {
                    console.log('Error scanning for devices : ', error);
                } 
              if (device && device.name == "TWN4 BLE"){
                    console.log('Device found : ',device.name);
                    console.log('Stop scanning.');
                    connectToDevice(device, userID);  
                }
            });
        } else {
            Alert.alert('Enter a valid user name.');
        }
    }
        
    const connectToDevice = async (device: Device, userID: String) => {
        try {
            bleManager.stopDeviceScan();
            const deviceConnection = await bleManager.connectToDevice(device.id);
            await deviceConnection.discoverAllServicesAndCharacteristics();
            console.log('Device connected : ', device.name);
            sendValue(device, userID);

        } catch (e) {
            console.log('FAILED TO CONNECT', e);
        }
    };

    const sendValue = async (device: Device, userID: String) => {
        try {
            const response = await bleManager.writeCharacteristicWithResponseForDevice(device.id, cardIdUUIDService, cardIdUUIDCharac, base64.encode(userID.toString()));
            if(userID.toString() === base64.decode(response.value).toString()) {
                console.log('Value send : ', userID);
                Alert.alert('User ID successfully sent to the card reader !');
            } else {
                console.log('Error : value not send : ', userID);
                Alert.alert('Error : user ID not sent correctly to the card reader !');
            }  
            disconnectFromDevice(device);  
        } catch (e) {
            console.log('FAILED TO SEND VALUE :', e);
        }
    };

    const disconnectFromDevice = (device: Device) => {
          bleManager.cancelDeviceConnection(device.id);
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

