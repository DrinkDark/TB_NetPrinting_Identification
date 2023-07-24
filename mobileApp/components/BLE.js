import React, { useState } from 'react';
import { Alert,Text, View, StyleSheet,PermissionsAndroid, Pressable, FlatList} from 'react-native';

import { BleManager, Characteristic, Device } from "react-native-ble-plx";

import { Buffer } from 'buffer';

import axios from 'axios';

import base64 from 'react-native-base64'

const bleManager = new BleManager();

const ipAddress = '192.168.137.1';

const CARD_ID_UUID_CHARAC = '495f449c-fc60-4048-b53e-bdb3046d4495';
const CARD_ID_UUID_SERVICE = '5a44c004-4112-4274-880e-cd9b3daedf8e';

var userID = '';

const Status = {
    NoAuthentication: 'NoAuthentication',
    DeviceAuthentication: 'DeviceAuthentication',
    WaitDeviceAuthentication: 'WaitDeviceAuthentication',
    DeviceAuthenticate: 'DeviceAuthenticate',
    WaitDeviceRandNum: 'WaitDeviceRandNum',
    AppAuthentication: 'AppAuthentication',
    WaitAppAuthentication: 'WaitAppAuthentication',
    AppAuthenticate: 'AppAuthenticate',
    Identification: 'Identification',
    Authenticate: 'Authenticate',
    AuthFailed: 'AuthFailed'
  };

var status = Status.NoAuthentication;

const BLE = () => {
    const [printedText, setPrintedText] = useState('');
    const [connectedDevice, setConnectedDevice] = useState();
    const [discoveredDeviceList, setDiscoveredDeviceList] = useState([]);
    const [modifiedCharac, setModifiedCharac] = useState('');
    const [randNum, setRandNum] = useState('');
    
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
    
    const onNotificationReceived = (
        error: BleError | null,
        characteristic: Characteristic | null,
      ) => {
        if (error) {
            console.log('Error notification (charachteristic : ' + characteristic.uuid + ')');
            console.log(error);
            return -1;
        } else if (!characteristic?.value) {
            console.log('Notification (charachteristic : ' + characteristic.uuid + ') received with no data');
            return -1;
        } else {
            console.log('Notification received (charachteristic : ' + characteristic.uuid + ') : ' + Buffer.from(characteristic.value, 'base64').toString('hex'));
            setModifiedCharac(Buffer.from(characteristic.value, 'base64').toString('hex'));

            switch(status) {
                case Status.WaitDeviceAuthentication:
                    status = Status.DeviceAuthentication;
                break;

                case Status.WaitDeviceRandNum:
                    status = Status.AppAuthentication;
                    break;
    
                case Status.WaitAppAuthentication:
                    status = Status.AppAuthenticate;
                    break;

                default:
                    break;
            }
        }
    
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
        if(userID !== '' && status === Status.NoAuthentication){
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
                    } else if (device.rssi >= -35){
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
            setConnectedDevice(device);

            setPrintedText(device.name + ' connected (' + device.id + ')');
            await device.discoverAllServicesAndCharacteristics();

            console.log('Enable monitor notification.');
            await device.monitorCharacteristicForService(CARD_ID_UUID_SERVICE, CARD_ID_UUID_CHARAC, (error, characteristic) => onNotificationReceived(error, characteristic));

            authenticationControler(device);  
            
        } catch (e) {
            console.log('FAILED TO CONNECT :', e);
        }
    };

    const getRandomNum = async(device) => {
        try {
            const response = await axios.get(`http://${ipAddress}:8080/getRandNum`);
            const randNum = response.data.randNum;
            setRandNum(randNum);
            console.log('App random number: ' + randNum);

            return randNum;
        } catch (error) {
            console.error(error);
            return null; // Return null or handle the error appropriately
        }
    };

    const getDecryptedData = (data) => {
        axios.get(`http://${ipAddress}:8080/getDecryptedData?data=${modifiedCharac}`)
        .then(response => {
            console.log('Decrypted random number : ' + response.data.decryptedData);
            return response.data.decryptedData;
        })
        .catch(error => {
            console.error(error);
            status = Status.AuthFailed;
        });

    };

    const getEncryptedData = (data) => {
        axios.get(`http://${ipAddress}:8080/getEncryptedData?data=${modifiedCharac}`)
        .then(response => {
            console.log('Encrypted random number : ' + response.data.encryptedData);
            response.data.encryptedData;
        })
        .catch(error => {
            console.error(error);
            status = Status.AuthFailed;
        });
    };
    

    const authenticationControler = async(device) => {
        while(true){
            switch(status) {
                case Status.NoAuthentication:
                    if(sendValue(device, await getRandomNum())){
                        console.log('Authentication protocole started...');
                        status = Status.WaitDeviceAuthentication;
                    } else {
                        status = Status.AuthFailed;
                    }
                    break;

                case Status.DeviceAuthentication:
                    if(randNum === getDecryptedData(modifiedCharac)) {
                        console.log('Device authenticate');
                        status = Status.DeviceAuthenticate;
                    } else{
                        status = Status.AuthFailed;
                    }
                    break;

                case Status.DeviceAuthenticate:
                    if(await sendValue(device, getRandomNum())){;
                        status = Status.WaitDeviceRandNum;
                    } else {
                        status = Status.AuthFailed;
                    }
                    break;

                case Status.WaitDeviceRandNum:

                    break;

                case Status.AppAuthentication:
                    if(await sendValue(device, getEncryptedData(modifiedCharac))){;
                        status = Status.WaitAppAuthentication;
                    } else {
                        status = Status.AuthFailed;
                    }
                    break;
    
                case Status.WaitAppAuthentication:

                    break;

                case Status.AppAuthenticate:
                   
                    break;
    
                case Status.Authenticate:
                    Alert.alert('User ID successfully sent to the card reader !');
                    console.log('Authentication done, ID send !');
                    setRandNum('');
                    disconnectFromDevice(device);
                    return;
    
                case Status.AuthFailed:
                    Alert.alert('Authentication failed !');
                    console.log('Authentication failed !');
                    disconnectFromDevice(device)
                    return;

                default:
                    break;
            }
        }
           
    };

    const sendValue = async (device, value) => {
        try {
            const response = await bleManager.writeCharacteristicWithResponseForDevice(device.id, CARD_ID_UUID_SERVICE, CARD_ID_UUID_CHARAC, base64.encode(value.toString()));
            if(value.toString() === base64.decode(response.value).toString()) {
                console.log('Value send : ', value);  
            } else {
                console.log('Value send failed: ', value);
                status = Status.AuthFailed;
            }  
        } catch (e) {
            console.log('FAILED TO SEND VALUE :', e);
        }
    };

    const disconnectFromDevice = (device) => {
        try {
            bleManager.cancelDeviceConnection(device.id);
            setConnectedDevice();
            clearDeviceList();
            status = Status.NoAuth;

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
        <View>
         <Text style={styles.message}>{discoveredDeviceList.length === 0 ? null : 'Approach the smartphone to the reader.'}</Text>
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
    message: {
        fontSize: 18,
        marginTop: 10,
        marginBottom: 15,
    },
      
})