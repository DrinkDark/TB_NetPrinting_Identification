/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, {useState, useEffect} from 'react';
import type {PropsWithChildren} from 'react';
import {
  StyleSheet,
  Text,
  useColorScheme,
  View,
  Image,
  Platform,
  NativeEventEmitter,
  PermissionsAndroid,
} from 'react-native';

import {
  Colors,
  DebugInstructions,
  Header,
  LearnMoreLinks,
  ReloadInstructions,
} from 'react-native/Libraries/NewAppScreen';

import BleManager from 'react-native-ble-manager';

import Button from './components/Button';

function App(): JSX.Element {

  useEffect(() => {
    
    BleManager.start({showAlert: false}).then(() => {
      console.log('BLE Manager initialized');
    });

    BleManager.enableBluetooth().then(() => {
      console.log('Bluetooth is turned on!');
    });

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
    }

  }, []);

  const isDarkMode = useColorScheme() === 'dark';

  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
  };

  return (
  <View style={styles.container}>
    <View style={styles.imageContainer}>
      <View>
        <Text style={styles.title}>NetPrinting identification</Text>
      </View>
      <Image
          source={require('./assets/images/printer.jpeg')}
          style={styles.image}
        />
      <View style={styles.footerContainer}>
          <Button theme="primary" label="Connect to device" />
          <Button label="Get user name" />
        </View>
      <View>
        <Text style={styles.credit}>Test application for netPrinting identification{'\n'} HEI Sion - Adrien Rey</Text>
      </View>
    </View>
  </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  title: {
    fontSize: 32,
    textDecorationLine: 'underline',
    marginTop: -60,
  },
  credit: {
    fontSize: 12,
    fontWeight: '400',
    textAlign: 'center',
    marginTop:30,
  },
  highlight: {
    fontWeight: '700',
  },
  image: {
    width: 200,
    height: 200,
    borderRadius: 18,
    marginTop: 20,
  },  
  imageContainer: {
    marginTop: 50,
    justifyContent: 'center',
    alignItems: 'center',
  },
  footerContainer: {
    flex: 9/10,
    alignItems: 'center',
    justifyContent: 'flex-end',
  }
});

export default App;
