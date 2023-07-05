import React from 'react';
import { Alert,Text, View, StyleSheet,PermissionsAndroid, Pressable} from 'react-native';
import { BleManager, Device } from "react-native-ble-plx";

import base64 from 'react-native-base64'
import useDevice from '../hooks/useDevice';

const bleManager = new BleManager();

const CARD_ID_UUID_CHARAC = '495f449c-fc60-4048-b53e-bdb3046d4495';
const CARD_ID_UUID_SERVICE = '5a44c004-4112-4274-880e-cd9b3daedf8e';

var userID = '';

const BLE = () => {
    const [connectedDevice, setConnectedDevice, printedText, setPrintedText] = useDevice();
    

    requestPermissions = async () => {
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
        if(grantedStatus == PermissionsAndroid.RESULTS.GRANTED){
            scanForDevices();
        } else {
            console.log('Error : Permission not granted.');
            Alert.alert('Permission not granted !');
        }
    };

    scanForDevices = () => {
        if(userID !== ''){
            console.log("Start scanning...");
            setPrintedText('Scanning...');
    
            bleManager.startDeviceScan(null, null, (error, device) => {
                if(error) {
                    console.log('Error scanning for devices : ', error);
                } 
                if (device && device.name == "TWN4 BLE"){
                    console.log('Device found : ', device.name);
                    console.log('Stop scanning.');
                    
                    connectToDevice(device);  
                }
            });
        } else {
            Alert.alert('Enter a valid user name.');
        }
    };

    connectToDevice = async (device) => {
        try {
            bleManager.stopDeviceScan();
            const deviceConnection = await bleManager.connectToDevice(device.id);
            console.log(deviceConnection),
            await setConnectedDevice(deviceConnection);

            setPrintedText(deviceConnection.name + ' connected (' + deviceConnection.id + ')');
            await deviceConnection.discoverAllServicesAndCharacteristics();

            sendValue();
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
                console.log('Error : ID not send.');
                Alert.alert('Error : user ID not sent correctly to the card reader !');
            }  
            disconnectFromDevice();  
        } catch (e) {
            console.log('FAILED TO SEND VALUE :', e);
            disconnectFromDevice(); 
            Alert.alert('Error : user ID not sent correctly to the card reader !');
             
        }
    };

    const disconnectFromDevice = () => {
        try {
            bleManager.cancelDeviceConnection(connectedDevice.id);
            console.log('Device disconnected : ' + connectedDevice.name);
            setPrintedText('');
        } catch (e) {
            console.log('FAILED TO DISCONNECT :', e);
        }    
    };

    return (
        <><View style={styles.container}>
        <View style={[styles.buttonContainer, { borderWidth: 4, borderColor: "#ffd33d", borderRadius: 18 }]}>
            <Pressable
                style={[styles.button, { backgroundColor: "#fff" }]}

                onPress={() => {
                    if(userID !== ''){
                        requestPermissions();
                    } else {
                        Alert.alert('Enter a valid user name !')
                    }
                    
                } }>
                <Text style={[styles.buttonLabel, { color: "#25292e" }]}>Authentication</Text>
            </Pressable>
        </View>
         <View>
         <Text style={styles.text}>{printedText}</Text>
         </View>
     </View></>
    );

};

BLE.setUserID = (id) => {
    userID = id;
};

export default BLE;

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        marginTop:50
    }, 
    text: {
        color: '#fff',
        fontSize: 16,
        marginBottom: 6,
    },
    buttonContainer: {
        width: 320,
        height: 68,
        marginHorizontal: 20,
        alignItems: 'center',
        justifyContent: 'center',
        padding: 3,
      },
      button: {
        borderRadius: 10,
        width: '100%',
        height: '100%',
        alignItems: 'center',
        justifyContent: 'center',
        flexDirection: 'row',
      },
      buttonIcon: {
        paddingRight: 8,
      },
      buttonLabel: {
        color: '#fff',
        fontSize: 20,
      },
})