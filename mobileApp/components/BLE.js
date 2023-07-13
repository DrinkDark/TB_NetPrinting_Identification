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
        sortDeviceList();             
    };

    const sortDeviceList = () => {
        setDiscoveredDeviceList(prevDeviceList => [...prevDeviceList].sort((a, b) => b.rssi - a.rssi));
    }

    const updateDevice = (updatedDevice) => {
        const updatedDeviceList = discoveredDeviceList.map(device => {
          if (device.id === updatedDevice.id) {
            return { ...device, ...updatedDevice };
          }
          return device;
        });
        setDiscoveredDeviceList(updatedDeviceList);
        sortDeviceList();
    };

    const clearDeviceList = () => {
        setDiscoveredDeviceList([]);
    };

    alreadyDiscover = (device) => {
        var exist = false;

        for (var i = 0; i < discoveredDeviceList.length; i++) {
            if (discoveredDeviceList[i].id === device.id) {
                updateDevice(device);
                //console.log('Device updated : ', device.name, ' (' , device.id, '), RSSI = ', device.rssi);
                exist = true;
                break; 
            }
        }
        return exist;
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

    scanForDevices = async () => {
        if(userID !== ''){
            console.log("Start scanning...");
            setPrintedText('Discovering ...');

            bleManager.startDeviceScan(null, null, (error, device) => {
                if(error) {
                    console.log('Error scanning for devices : ', error);
                } 
                if (device && device.serviceUUIDs && (device.serviceUUIDs).includes(CARD_ID_UUID_SERVICE)){
                    if(alreadyDiscover(device) === false){
                        console.log('Device found : ', device.name, ' (' , device.id, '), RSSI = ', device.rssi);
                        addDevice(device);
                    } else if (device.rssi >= -30){
                        connectToDevice(device);
                    }
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

            setPrintedText(device.name + ' connected (' + device.id + ')');
            await deviceConnection.discoverAllServicesAndCharacteristics();

            sendValue(device);
            
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

    const Item = ({title, id}) => (
        <View style={[styles.item, discoveredDeviceList[0].id == id && styles.itemSelected]}>
          <Text style={styles.itemText}>{title}</Text>
        </View>
    );

    return (
        <><View style={styles.container}>
        <Text style={styles.itemTitle}>{discoveredDeviceList.length === 0 ? null : 'Discovered devices'}</Text>
        <View style={styles.containerFlatlist}>
            <FlatList
                data={discoveredDeviceList}
                renderItem={({item}) => <Item title={item.name + ' (' + item.id + ')'} id={item.id} />}
                keyExtractor={item => item.id}
            />
        </View>
       
        <View style={[styles.buttonContainer, { borderWidth: 4, borderColor: "#ffd33d", borderRadius: 18 }]}>
            <Pressable
                style={[styles.button, { backgroundColor: "#fff" }]}
                onPress={() => {
                    if(userID !== ''){
                        requestPermissions();
                    } else {
                        Alert.alert('Select a user !')
                    }
                } }>
                <Text style={[styles.buttonLabel, { color: "#25292e" }]}>{'Discover devices' }</Text>
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
        flex: 2,
        justifyContent: 'center',
        alignItems: 'center',
        marginTop: 30,
        marginBottom: 20,

    }, 
    containerFlatlist: {
        flex: 1,
        marginBottom: 20,
    }, 
    item: {
        padding: 8,
        backgroundColor: '#404040',
        marginBottom: 5,
        marginVertical: 8,
        borderRadius: 10,
        alignItems: 'center',
      },
    itemSelected: {
        padding: 8,
        backgroundColor: '#7390ad',
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
        fontSize: 22,
        fontWeight: 'bold',
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
    text: {
        fontSize: 16,
        marginTop: 10,
    },
      
})