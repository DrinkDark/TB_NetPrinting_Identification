import React from 'react';
import {Text, View, StyleSheet} from 'react-native';

import useUser from '../hooks/useUser';
import useDevice from '../hooks/useDevice';

export const requestPermissions = async () => {
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
};

export const scanForDevices = () => {
    const [userName, onChangeUserName, userID] = useUser(); 
    const [allDevices, setAllDevices, connectedDevice, setConnectedDevice, printedText, setPrintedText] = useDevice();

    if(userID !== ''){
        console.log("Start scanning...");
        setPrintedText('Scanning...');

        bleManager.startDeviceScan(null, null, (error, device) => {
            if(error) {
                console.log('Error scanning for devices : ', error);
            } 
            if (device && device.name == "TWN4 BLE"){
                setAllDevices((prevState) => {
                    if (!isDuplicteDevice(prevState, device)) {
                        return [...prevState, device];
                    }
                    return prevState;
                });

                console.log('Device found : ', device.name);
                console.log('Stop scanning.');
                
                connectToDevice(device);  
            }
        });
    } else {
        Alert.alert('Enter a valid user name.');
    }
};

const BLE = () => {
    const [userName, onChangeUserName, userID] = useUser(); 
    const [allDevices, setAllDevices, connectedDevice, setConnectedDevice, printedText, setPrintedText] = useDevice();
    

    const isDuplicteDevice = (devices, nextDevice) =>
    devices.findIndex(device => nextDevice.id === device.id) > -1;

    connectToDevice = async (device) => {
        try {
            bleManager.stopDeviceScan();
            const deviceConnection = await bleManager.connectToDevice(device.id);
            setConnectedDevice(deviceConnection);

            console.log('Device connected : ', connectedDevice.name);
            setPrintedText(connectedDevice.name,' connected (', connectedDevice.id,')');
            await deviceConnection.discoverAllServicesAndCharacteristics();

            sendValue(userID);
        } catch (e) {
            console.log('FAILED TO CONNECT :', e);
        }
    };

    const sendValue = async () => {
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
            setPrintedText('');
        } catch (e) {
            console.log('FAILED TO DISCONNECT :', e);
        }    
    };

    return (
        <View style={styles.container}>
        <Text style={styles.text}>{printedText}</Text>
        </View>
    );

};

export default BLE;

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
    }, 
    text: {
        color: '#fff',
        fontSize: 16,
        marginBottom: 6,
    },
})