/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, { useEffect, useRef, useState } from 'react';

import {
  StyleSheet,
  Text,
  useColorScheme,
  View,
  Image,
  LogBox,
  TextInput,
  ScrollView,

} from 'react-native';

import {
  Colors,

} from 'react-native/Libraries/NewAppScreen';

import { Device } from "react-native-ble-plx";

import Button from './components/Button';
import axios from 'axios';

LogBox.ignoreLogs(['new NativeEventEmitter']); // Ignore log notification by message

function App(): JSX.Element {
  const scrollViewRef = useRef();

  const scrollToInput = () => {
    scrollViewRef.current.scrollToEnd({ animated: true });
  };
  const [userName, onChangeUserName] = useState('');
  const [userID, onChangeUserID] = useState('');
  const isDarkMode = useColorScheme() === 'dark';

  useEffect(() => {
    if (userName !== '') {
      axios.get('http://10.93.11.8:8080/getUserID?data=' + userName)
      .then(response => {
        const userID = response.data;
        onChangeUserID(userID);
        console.log('UserID : ' + userID);
      })
      .catch(error => {
        console.error(error);
      });
    } else {
      onChangeUserID('');
    }
  }, [userName]);
  
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
      <View style={styles.imageContainer}>
      <Image
          source={require('./assets/images/printer.jpeg')}
          style={styles.image}
        />
      </View>
      <View style={styles.footerContainer}>
          <Button theme="primary" label="Connect to device" />
          <Button label="Get user name" />
        </View>
      <View>
        <Text style={styles.credit}>Test application for netPrinting identification{'\n'} HEI Sion - Adrien Rey</Text>
      </View>
    </ScrollView>
  </View>
   
  );
}

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
    marginTop:30,
  },
  image: {
    width: 200,
    height: 200,
    borderRadius: 18,
    marginTop: 30,
  },  
  imageContainer: {
    marginTop: 50,
    justifyContent: 'center',
    alignItems: 'center',
  },
  footerContainer: {
    marginTop: 40,
    alignItems: 'center',
    justifyContent: 'flex-end',
  }
});

export default App;


