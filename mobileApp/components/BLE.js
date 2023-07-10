import React, { useState } from 'react';
import { Alert,Text, View, StyleSheet,PermissionsAndroid, Pressable, FlatList} from 'react-native';
import { BleManager, Device } from "react-native-ble-plx";

import base64 from 'react-native-base64'

const bleManager = new BleManager();

const CARD_ID_UUID_CHARAC = '495f449c-fc60-4048-b53e-bdb3046d4495';
const CARD_ID_UUID_SERVICE = '5a44c004-4112-4274-880e-cd9b3daedf8e';

var userID = '';

const BLE = () => {
    const [printedText, setPrintedText] = useState('');
    const [connectedDevice, setConnectedDevice] = useState();
    const [discoveredDeviceList, setDiscoveredDeviceList] = useState([]);

    const addDevice = (device) => {
        setDiscoveredDeviceList(prevDeviceList => [...prevDeviceList, device]);
    };

    const removeDevice = (deviceId) => {
        setDiscoveredDeviceList(prevDeviceList => prevDeviceList.filter(device => device.id !== deviceId));
    };

    const clearDeviceList = () => {
        setDiscoveredDeviceList([]);
    };

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
            await scanForDevices();
        } else {
            console.log('Error : Permission not granted.');
            Alert.alert('Permission not granted !');
        }
    };

    alreadyDiscover = (device) => {
        var exist = false;

        for (var i = 0; i < discoveredDeviceList.length; i++) {
            if (discoveredDeviceList[i].id === device.id) {
                exist = true;
                break; 
            }
        }
        return exist;
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
                    if(alreadyDiscover(device) === false){
                        console.log('Device found : ', device.name);
                        addDevice(device);
                    }
                    //connectToDevice(device);  
                }
            });
        } else {
            Alert.alert('Enter a valid user name.');
        }
    };

    connectToDevice = async (device) => {
        try {
            bleManager.stopDeviceScan();
            console.log('Stop scanning.');
            const deviceConnection = await bleManager.connectToDevice(device.id);
            setConnectedDevice(deviceConnection);

            setPrintedText(connectedDevice + ' connected (' + connectedDevice + ')');
            await deviceConnection.discoverAllServicesAndCharacteristics();

            sendValue(connectedDevice);
        } catch (e) {
            console.log('FAILED TO CONNECT :', e);
        }
    };

    const sendValue = async (device) => {
        try {
            const response = await bleManager.writeCharacteristicWithResponseForDevice(device.id, CARD_ID_UUID_SERVICE, CARD_ID_UUID_CHARAC, base64.encode(userID.toString()));
            if(userID.toString() === base64.decode(response.value).toString()) {
                console.log('Value send : ', userID);
                Alert.alert('User ID successfully sent to the card reader !');
            } else {
                console.log('Error : ID not send.');
                Alert.alert('Error : user ID not sent correctly to the card reader !');
            }  
            
        } catch (e) {
            console.log('FAILED TO SEND VALUE :', e);
            Alert.alert('Error : user ID not sent correctly to the card reader !');  
        }
        disconnectFromDevice(device);  
    };

    const disconnectFromDevice = (device) => {
        try {
            bleManager.cancelDeviceConnection(device.id);
            setConnectedDevice();
            clearDeviceList();

            console.log('Device disconnected : ' + device.name);
            setPrintedText('');
        } catch (e) {
            console.log('FAILED TO DISCONNECT :', e);
        }    
    };

    const Item = ({title}) => (
        <View style={styles.item}>
          <Text style={styles.itemText}>{title}</Text>
        </View>
    );

    return (
        <><View style={styles.container}>
        <FlatList
            ListHeaderComponent={discoveredDeviceList.length > 0 && <Text style={styles.itemTitle}>Discovered devices : </Text>}
            data={discoveredDeviceList}
            renderItem={({item}) => <Item title={item.name + ' (' + item.id + ')'} />}
            keyExtractor={item => item.id}
        />
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
                <Text style={[styles.buttonLabel, { color: "#25292e" }]}>Start scanning</Text>
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
        marginTop: 30
    }, 
    item: {
        padding: 8,
        backgroundColor: '#404040',
        marginBottom: 5,
        marginVertical: 8,
        borderRadius: 10,
        alignItems: 'center',
      },
    itemText: {
        color: '#fff',
        fontSize: 16,
        marginHorizontal: 10,
    },
    itemTitle: {
        color: '#fff',
        fontSize: 20,
        marginBottom: 6,
        textDecorationLine: 'underline',
        textAlign: 'center', // Center text horizontally
    
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