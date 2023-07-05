/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, { useEffect, useRef, useState } from 'react';
import useUser from './hooks/useUser'
import useEncryption from './hooks/useEncryption';
import Button from './components/Button';
import BLE from './components/BLE';

import {
  StyleSheet,
  Text,
  useColorScheme,
  View,
  LogBox,
  TextInput,
  ScrollView,
  Switch,

} from 'react-native';

import {
  Colors,

} from 'react-native/Libraries/NewAppScreen';

const App = () => {
  const [userName, onChangeUserName, userID] = useUser();
  const [plainText, cipherText, encryptData, decryptData] = useEncryption();

  const scrollViewRef = useRef();

  const scrollToInput = () => {
    scrollViewRef.current.scrollToEnd({ animated: true });
  };

  const isDarkMode = useColorScheme() === 'dark';

  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
  };

  
  return (
  
  <View style={styles.container}>
  <ScrollView ref={scrollViewRef} contentContainerStyle={styles.scrollViewContent}>
      <View>
        <Text style={styles.title}>NetPrinting identification</Text>
      </View>
      <View style={styles.containerUserName}>
        <Text style={styles.text}>  User name : </Text>
        <TextInput
          style={styles.input}
          onChangeText={onChangeUserName}
          value={userName}
          placeholder="Enter user name"
          keyboardType="default"
        />
      </View>
      <View style={styles.containerUserName}>
        <Text style={styles.text}>  User ID : {userID}</Text>
      </View>
      <View style={styles.containerUserName}>
        <Text style={styles.text}>  Encrypt data : {cipherText}</Text>
      </View>
      <BLE></BLE>
      <View style={styles.footerContainer}>
          <Button theme="authentication" label="Authentication" userID={userID}/>
          <Button theme="encrypt" label='Encrypt data'/>
          <Button theme="decrypt" label='Decrypt data'/>
        </View>
      <View>
        <Text style={styles.credit}>Test application for netPrinting identification{'\n'} HEI Sion - Adrien Rey</Text>
      </View>
    </ScrollView>
  </View>
   
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  containerUserName: {
    flexDirection: 'row',
    alignItems: 'center',
    borderColor: 'gray',  
    marginTop: 35,  
  },
  title: {
    fontSize: 34,
    textDecorationLine: 'underline',
    marginTop: 10,
    fontWeight: 'bold',
  },
  text: {
    fontSize: 24,
    marginBottom: 6,
  },
  input: {
    height: 40,
    fontSize: 20,
    padding: 10,
    marginTop: 2,
  },
  credit: {
    fontSize: 12,
    fontWeight: '400',
    textAlign: 'center',
    marginTop:40,
  },
  containerSwitch: {
    flexDirection: 'row',
    alignItems: 'center',

    marginTop: 20,
  },
  footerContainer: {
    marginTop: 80,
    alignItems: 'center',
    justifyContent: 'flex-end',
  }
});

export default App;



