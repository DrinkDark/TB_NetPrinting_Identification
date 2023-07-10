/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, { useEffect, useRef, useState } from 'react';
//import useEncryption from './hooks/useEncryption';
import BLE from './components/BLE';

import {
  StyleSheet,
  Text,
  useColorScheme,
  View,
  ScrollView,
} from 'react-native';

import {
  Colors,

} from 'react-native/Libraries/NewAppScreen';
import User from './components/User';

const App = () => {
  //const [plainText, cipherText, encryptData, decryptData] = useEncryption();

  const scrollViewRef = useRef();

  const isDarkMode = useColorScheme() === 'dark';

  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
  };

  return (
    <View style={styles.container}>
        <View>
          <Text style={styles.title}>NetPrinting identification</Text>
        </View>
      <User></User>
      <BLE></BLE>
        <View>
          <Text style={styles.credit}>Test application for netPrinting identification{'\n'} HEI Sion - Adrien Rey</Text>
        </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  title: {
    fontSize: 32,
    textDecorationLine: 'underline',
    marginTop: 20,
    fontWeight: 'bold',
  },
  text: {
    fontSize: 24,
    marginBottom: 6,
  },
  credit: {
    fontSize: 12,
    fontWeight: '400',
    textAlign: 'center',
    marginTop:10,
    marginBottom: 20,
  },
});

export default App;


