///////////////////////////////////////////////////
//           Application TSX component
///////////////////////////////////////////////////

import React, { useRef } from 'react';

import {
  StyleSheet,
  Text,
  useColorScheme,
  View,
} from 'react-native';

import {
  Colors,
} from 'react-native/Libraries/NewAppScreen';

import User from './components/User';
import BLE from './components/BLE';

/**
 * // App component constructor
 * 
 * @returns app JSX component
 */
const App = () => {
  const scrollViewRef = useRef();   //  

  const isDarkMode = useColorScheme() === 'dark';   // Define theme

  /**
   * Define background color
   */
  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
  };


  /**
   * Return the JSX element (UI component)
   * 
   * Contain the application title, custom user and ble component and the application credit
   * The two custom components are defined in others files and render her
   */
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


/**
 * Style sheet containing styles for the JSX element
 */
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
    marginTop: 15,
    marginBottom: 30,
  },
});

export default App;


