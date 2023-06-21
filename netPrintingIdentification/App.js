import React, {useState, useEffect} from 'react';
import { StatusBar } from 'expo-status-bar';
import { 
  StyleSheet, 
  View, 
  Platform,
  NativeModules,
  useColorScheme,
  NativeEventEmitter,
} from 'react-native';

import ImageViewer from './components/ImageViewer'; 
import Button from './components/Button';
//import Authentication from './components/Authentication';
import BleManager from 'react-native-ble-manager';

const PlaceholderImage = require('./assets/images/printer.jpeg');
const BleManagerModule = NativeModules.BleManager;
const BleManagerEmitter = new NativeEventEmitter(BleManagerModule);

//const bleManager = new BleManager();

export default function App() {
  useEffect(() => {
    BleManager.start({ showAlert: false}).then(() => {
      console.log("Module initialized");
    }) 
    BleManager.enableBluetooth().then(() => {
      console.log('Bluetooth is turned on!');
    })
    
    /*let stopListener = BleManagerEmitter.addListener(
      'BleManagerStopScan',
      () => {
        setIsScanning(false);
        console.log('Scan is stopped');
        handleGetConnectedDevices();
      },
    );

    if (Platform.OS === 'android' && Platform.Version >= 23) {
      PermissionsAndroid.check(
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
      ).then(result => {
        if (result) {
          console.log('Permission is OK');
        } else {
          PermissionsAndroid.request(
            PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
          ).then(result => {
            if (result) {
              console.log('User accept');
            } else {
              console.log('User refuse');
            }
          });
        }
      });
    }*/
    return () => {
      //stopListener.remove();
    };
  }, []);

  return (
    <View style={styles.container}>

      <View style={styles.imageContainer}>
        <ImageViewer placeholderImageSource={PlaceholderImage} />
      </View>
      <View style={styles.footerContainer}>
        <Button theme="primary" label="Connect to printer" />
      </View>
      <View style={styles.footerContainer}>
        <Button theme="secondary" label="Get user name" />
      </View>
      <StatusBar style="auto" />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#25292e',
    alignItems: 'center',
  },
  title: {
    fontSize: 32,
    fontWeight: 'bold',
    color: "white",
    marginTop: 10,
  },
  imageContainer: {
    flex: 1,
    paddingTop: 58,
    marginTop: 60,
  },  
  footerContainer: {
    flex: 1/10,
    alignItems: 'center',
    justifyContent: 'flex-end',
    marginBottom: 50,
  },
});

