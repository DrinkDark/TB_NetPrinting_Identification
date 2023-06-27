/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, { useState } from 'react';

import {
  StyleSheet,
  Text,
  useColorScheme,
  View,
  Image,
  LogBox,

} from 'react-native';

import {
  Colors,

} from 'react-native/Libraries/NewAppScreen';

import { Device } from "react-native-ble-plx";

import Button from './components/Button';
import useBle from './useBLE';

LogBox.ignoreLogs(['new NativeEventEmitter']); // Ignore log notification by message

function App(): JSX.Element {

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
  text: {
    marginTop: 30,
    fontSize: 24,
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
