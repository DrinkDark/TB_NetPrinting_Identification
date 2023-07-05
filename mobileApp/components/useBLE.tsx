import { Alert, PermissionsAndroid, Text} from "react-native";
import { BleManager, Device } from "react-native-ble-plx";

import base64 from 'react-native-base64'
import useUser from "../hooks/useUser";
import { useState } from "react";

type PermissionCallback = (result: boolean) => void;

const bleManager = new BleManager();

const CARD_ID_UUID_CHARAC = '495f449c-fc60-4048-b53e-bdb3046d4495';
const CARD_ID_UUID_SERVICE = '5a44c004-4112-4274-880e-cd9b3daedf8e';

const useBLE = () => {
    //const user = useUser();
    var connectedDevice: Device;

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
                connectedDevice = device;
                console.log('Device found : ', device.name);
                console.log('Stop scanning.');
                connectToDevice(userID);  
                }
            });
        } else {
            Alert.alert('Enter a valid user name.');
        }
    }
        
    const connectToDevice = async (userID: String) => {
        try {
            bleManager.stopDeviceScan();
            const deviceConnection = await bleManager.connectToDevice(connectedDevice.id);
            await deviceConnection.discoverAllServicesAndCharacteristics();
            console.log('Device connected : ', connectedDevice.name);

            sendValue(userID);
        } catch (e) {
            console.log('FAILED TO CONNECT :', e);
        }
    };

    const sendValue = async (userID: String) => {
        try {
            const response = await bleManager.writeCharacteristicWithResponseForDevice(connectedDevice.id, CARD_ID_UUID_SERVICE, CARD_ID_UUID_CHARAC, base64.encode(userID.toString()));
            if(userID.toString() === base64.decode(response.value).toString()) {
                console.log('Value send : ', userID);
                Alert.alert('User ID successfully sent to the card reader !');
            } else {
                console.log('Error : value not send : ', userID);
                Alert.alert('Error : user ID not sent correctly to the card reader !');
            }  
            disconnectFromDevice();  
        } catch (e) {
            console.log('FAILED TO SEND VALUE :', e);
        }
    };

    const disconnectFromDevice = () => {
        try {
            bleManager.cancelDeviceConnection(connectedDevice.id);
            console.log('Device disconnected : ', connectedDevice.name);
        } catch (e) {
            console.log('FAILED TO DISCONNECT :', e);
        }
          
      };

    /*return {
        requestPermissions,
        scanForDevices,
    };*/
    return (
        <Text>Hello world ! </Text>
   );
};

function requestMultiple(arg0: any[]): any {
    throw new Error("Function not implemented.");
}

export default useBLE;

