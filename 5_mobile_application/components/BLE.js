///////////////////////////////////////////////////
//             BLE custom component
///////////////////////////////////////////////////

import React, { useState } from 'react';
import { 
    Alert,
    Text, 
    View, 
    StyleSheet,
    PermissionsAndroid, 
    Pressable, 
    FlatList
} from 'react-native';

import { BleManager, Characteristic } from "react-native-ble-plx";
import { Buffer } from 'buffer';
import axios from 'axios';
import base64 from 'react-native-base64'

const bleManager = new BleManager();

const ipAddress = '192.168.137.1';      // Middleware component address

const SERVICE_UUID = '5a44c004-4112-4274-880e-cd9b3daedf8e';    // Service UUID
const CHARAC_UUID = '495f449c-fc60-4048-b53e-bdb3046d4495';     // Characteristic UUID


var userID = '';        // User ID 
var connectedDevice;    // Connected device
var modifiedCharac;     // Modified characteristic value
var randNum;            // Random number value

// SM states
const States = {
    ST_OnIdle: 'ST_OnIdle',
    ST_StartAuthentication: 'ST_StartAuthentication',
    ST_DeviceAuthentication: 'ST_DeviceAuthentication',
    ST_WaitDeviceAuthentication: 'ST_WaitDeviceAuthentication',
    ST_DeviceAuthenticated: 'ST_DeviceAuthenticated',
    ST_WaitDeviceRandNum: 'ST_WaitDeviceRandNum',
    ST_AppAuthentication: 'ST_AppAuthentication',
    ST_WaitAppAuthentication: 'ST_WaitAppAuthentication',
    ST_AppAuthenticated: 'ST_AppAuthenticated',
    ST_WaitIdentification: 'ST_WaitIdentification',
    ST_Identify: 'ST_Identify',
    ST_AuthenticationFailed: 'AuthFailed'
  };

var currentState = States.ST_OnIdle;    // Current state


/**
 * BLE component constructor
 * 
 * @returns BLE component
 */
const BLE = () => {
    const [printedText, setPrintedText] = useState('');                     // Printed text hook
    const [discoveredDeviceList, setDiscoveredDeviceList] = useState([]);   // Discover device list hoook

    
    /**
     * Add a new device to the discover device list
     * 
     * @param {*} device device to add
     */
    const addDevice = (device) => {
        setDiscoveredDeviceList(prevDeviceList => [...prevDeviceList, device]);  
        sortDeviceList();             
    };

    /**
     * Sort the discover device list in the descending order using the device's rssi
     */
    const sortDeviceList = () => {
        setDiscoveredDeviceList(prevDeviceList => [...prevDeviceList].sort((a, b) => b.rssi - a.rssi));
    }
 
    /**
     * Update the discover device list with the new device values
     * 
     * @param {*} updatedDevice device to update 
     */
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

    /**
     * Clear discover device list
     */
    const clearDeviceList = () => {
        setDiscoveredDeviceList([]);
    };

    /**
     * Check if a device is already discover
     * 
     * Update device if already discover
     */
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
    
    // 
    /**
     * BLE notification handler
     * 
     * Called when a notification is received from a BLE characteristic.
     * 
     * @param {*} error received error
     * @param {*} characteristic received characteristic
     */
    const onNotificationReceived = (
        error: BleError | null,
        characteristic: Characteristic | null,
      ) => {
        // A error is received
        if (error) {
            //console.log('Error notification (charachteristic : ' + characteristic.uuid + ')'); 
            if(currentState != States.ST_OnIdle) {
                console.log(error);

                currentState = States.ST_AuthenticationFailed;
                chooseSMstate();
            }
  
        // The received value is empty
        } else if (!characteristic?.value) {
            //console.log('Notification (charachteristic : ' + characteristic.uuid + ') received with no data');
        
        // The received value is valid
        } else {
            //console.log('Notification received (charachteristic : ' + characteristic.uuid + ') : ' + Buffer.from(characteristic.value, 'base64').toString('hex'));
            modifiedCharac = Buffer.from(characteristic.value, 'base64').toString('hex');       // Decode the base64 recieved value

            // Select the SM state after receiving a notification
            switch(currentState) {
                case States.ST_WaitDeviceAuthentication:
                    currentState = States.ST_DeviceAuthentication;
                break;

                case States.ST_WaitDeviceRandNum:
                    currentState = States.ST_AppAuthentication;
                    break;
    
                case States.ST_WaitAppAuthentication:
                    currentState = States.ST_AppAuthenticated;
                    break;

                case States.ST_WaitIdentification:
                    currentState = States.ST_Identify;
                    break;
                    
                default:
                    break;
            }
            chooseSMstate();      // Start again the state machine after the notification
        }
      };

    /**
     * Request the permissions to use the location and BLE
     */
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

    /**
     * Scan for BLE device
     * 
     * Connect to a discover device if his rssi is greater than -40dB
     */
    scanForDevices = async () => {
        if(userID !== '' && currentState === States.ST_OnIdle){
            console.log("Start scanning...");
            setPrintedText('Discovering ...');

            // Start the device scanning
            bleManager.startDeviceScan(null, null, (error, device) => {
                if(error) {
                    console.log('Error scanning for devices : ', error);
                } 

                // If a device is discovered and the include the service UUID
                // the device is added to the discovered device list if not already discovered
                // If the rssi is bigger than -40dB, it try to connect to the device
                if (device && device.serviceUUIDs && (device.serviceUUIDs).includes(SERVICE_UUID)){
                    if(alreadyDiscover(device) === false){
                        console.log('Device found : ', device.name, ' (' , device.id, '), RSSI = ', device.rssi);
                        addDevice(device);
                    } else if (device.rssi >= -40){
                        connectToDevice(device);
                    }
                }
            });

        // No user is selected
        } else {
            Alert.alert('Select a user !');
        }
    };

    /**
     * Connect to the device pass in parameter
     */
    connectToDevice = async (device) => {
        try {
            bleManager.stopDeviceScan();    // Stop the device scan  
            console.log('Stop scanning.');

            // Try to connect to the device
            if(await bleManager.connectToDevice(device.id)){
                connectedDevice = device;       // Set the connected device

                setPrintedText(connectedDevice.name + ' connected (' + connectedDevice.id + ')');   // Set the printed text
                await connectedDevice.discoverAllServicesAndCharacteristics();                      // Discover all GATT server services and characteristics
    
                // Enable the notfication for the characteristics
                console.log('Enable monitor notification.');
                await connectedDevice.monitorCharacteristicForService(SERVICE_UUID, CHARAC_UUID, (error, characteristic) => onNotificationReceived(error, characteristic));
                
                currentState = States.ST_StartAuthentication;
                chooseSMstate(connectedDevice);  

            // Connection failed
            } else {
                Alert.alert('Authentication failed !');
                console.log('Connection failed !');
            }
            
        } catch (e) {
            console.log('FAILED TO CONNECT :', e);
        }
    };
 
    /**
     * Disconnect from device
     * 
     * Clear the device list and reset SM to idle state
     */
    const disconnectFromDevice = () => {
        try {
            if(bleManager.isDeviceConnected(connectedDevice.id))
            {
                bleManager.cancelDeviceConnection(connectedDevice.id);
            }
            clearDeviceList();
            currentState = States.ST_OnIdle;

            console.log('Device disconnected : ' + connectedDevice.name);
            setPrintedText('');
        } catch (e) {
            console.log('FAILED TO DISCONNECT :', e);
        }    
    };   

    /**
     * Write value in the characteristic
     * 
     * Write the value in base64.
     * Control if the received response's value correspond to the send value
     * 
     * @param {*} value value to write
     * @returns true if succeed, else false
     */
    const writeValue = async (value) => {
        try {
            const response = await bleManager.writeCharacteristicWithResponseForDevice(connectedDevice.id, SERVICE_UUID, CHARAC_UUID, base64.encode(value.toString()));
           
            // Check if write response correspond to the send value
            if(value.toString() === base64.decode(response.value).toString()) {
                //console.log('Value send : ', value);  
                return true;
            } else {
                console.log('Value send failed: ', value);
                return false;
            }  
        } catch (e) {
            console.log('FAILED TO SEND VALUE :', e);
            return false;
        }
    };

    /**
     * Get random number fro the middleware component
     * 
     * @returns received random nummber
     */
    const getRandomNum = async() => {
        try {
            const response = await axios.get(`http://${ipAddress}:8080/getRandNum`);
            randNum = response.data.randNum;
            
            //console.log('App random number: ' + randNum);
            return randNum;
        } catch (error) {
            console.error(error);
            currentState = States.ST_AuthenticationFailed;
        }
    };

    /**
     * Get decrypted data fro the middleware component
     * 
     * @param {*} data data to decrypt
     * 
     * @returns received decrypted data
     */
    const getDecryptedData = async(data) => {
        try {
            const response = await axios.get(`http://${ipAddress}:8080/getDecryptData?data=${data}`);
            const decryptData = response.data.decryptedData;

            //console.log('Decrypted data : ' + decryptData);
            return decryptData;
        } catch (error) {
            console.error(error);
            currentState = States.ST_AuthenticationFailed;
        }
    };

    
    /**
     * Get encrypted data fro the middleware component
     * 
     * @param {*} data data to encrypt
     * 
     * @returns received encrypted data
     */
    const getEncryptedData = async(data) => {
        try {
            const response = await axios.get(`http://${ipAddress}:8080/getEncryptData?data=${data}`);
            const encryptData = response.data.encryptedData;

            //console.log('Encrypted data : ' + encryptData);
            return encryptData;
        } catch (error) {
            console.error(error);
            currentState = States.ST_AuthenticationFailed;
        }
    };
    
    /**
     * Get signed message fro the middleware component
     * 
     * @returns received signed message
     */
    const getSignedMessage = async() => {
        try {
            const response = await axios.get(`http://${ipAddress}:8080/getSignedMessage?userID=${userID}`);
            
            //console.log('Signed message : ' + response.data.signedMessage);
            return response.data.signedMessage;
        } catch (error) {
            console.error(error);
            currentState = States.ST_AuthenticationFailed;
        }
    };

    /**
     * Choose state machine state
     * 
     * Contain a while loop to make the SM work
     * Have to quit this functions when a notification is waited.
     * Can't call onNotificationReceived functions if in the loop
     */
    const chooseSMstate = async() => {
        // While loop for the state machine
        while(true){
            switch(currentState) {

                // Write random number in the characteristic 
                case States.ST_StartAuthentication:
                    console.log('Authentication protocol started...');
                    if(await writeValue(await getRandomNum())){
                        currentState = States.ST_WaitDeviceAuthentication;
                        return;     // Quit this function. Wait a notification 
                    } else {
                        currentState = States.ST_AuthenticationFailed;
                    }
                    break;

                // Decrypt received data via notification and compare it with the send random number
                case States.ST_DeviceAuthentication:
                    if(randNum === await getDecryptedData(modifiedCharac)) {
                        currentState = States.ST_DeviceAuthenticated;
                    } else{
                        currentState = States.ST_AuthenticationFailed;
                    }
                    break;

                // Write random number to signify the device authentication succeed
                case States.ST_DeviceAuthenticated:
                    console.log('Device authenticated');
                    if(await writeValue(await getRandomNum())){;
                        currentState = States.ST_WaitDeviceRandNum;
                        return;     // Quit this function. Wait a notification
                    } else {
                        currentState = States.ST_AuthenticationFailed;
                    }
                    break;

                // Encrypt the received random number and write it in the characteristic 
                case States.ST_AppAuthentication:
                    if(await writeValue(await getEncryptedData(modifiedCharac))){;
                        currentState = States.ST_WaitAppAuthentication;
                        return;     // Quit this function. Wait a notification
                    } else {
                        currentState = States.ST_AuthenticationFailed;
                    }
                    break;

                // Write the signed message in the characteristic
                case States.ST_AppAuthenticated:
                    console.log('App authenticated');
                    if(await writeValue(await getSignedMessage())){;
                        currentState = States.ST_WaitIdentification;
                        return;     // Quit this function. Wait a notification
                    } else {
                        currentState = States.ST_AuthenticationFailed;
                    }
                break;

                // Authentication succeed
                // Disconnect from the device
                case States.ST_Identify:
                    Alert.alert('User ID successfully sent to the card reader !');
                    console.log('Authentication done, ID send !');
                    disconnectFromDevice(connectedDevice);

                    currentState = States.ST_OnIdle;
                    return;     // Quit this function. The authentication procedure is finish
    
                // Authentication procedure failed
                // Desconnect from the device
                case States.ST_AuthenticationFailed:
                    Alert.alert('Authentication failed !');
                    console.log('Authentication failed !');

                    if(connectedDevice.isConnected()) {
                        disconnectFromDevice(connectedDevice)
                    }
                    
                    return;     // Quit this function. The authentication procedure has failed

                default:
                    break;
            }
        }
           
    };       

    /**
     * Set user identifier
     * @param {*} id user identifier to set
     */
    BLE.setUserID = (id) => {
        userID = id;
    };

    // 
    /**
     * Define item for device list
     * 
     * @param {*} param title and the id of the item
     *  
     * @returns rendered item
     */
    const Item = ({title, id}) => (
        <View style={[styles.item, discoveredDeviceList[0].id == id && styles.itemSelected]}>
          <Text style={styles.itemText}>{title}</Text>
        </View>
    );

    /**
     * Return the BLE custom component
     * 
     * Contain the discover device list, a button to start discovering data and text
     */
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


export default BLE;     // Export custom BLE component

/**
 * Style sheet containing styles for the component
 */
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