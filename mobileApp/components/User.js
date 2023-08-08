///////////////////////////////////////////////////
//           User custom component
///////////////////////////////////////////////////

import React from 'react';

import {
    StyleSheet,
    Text,
    FlatList,
    View,
    TouchableOpacity,
  } from 'react-native';

import useUser from '../hooks/useUser'  


/**
 * User component constructor
 * 
 * @returns user component
 */
const User = () => {
    const [userName, onChangeUserName, userID, users] = useUser();    // Initialize useUser custom hook

      /**
       * Handle the press event when a user item is selected
       * 
       * @param {*} user pressed user
       */
      const handlePress = (user) => {
        if (userName === user) {
            onChangeUserName(null);   // Unselect current user if pressed
        } else {
            onChangeUserName(user);   // Changed user if new pressed
        }
      };
    
      //
      // 
      /**
       * Render each item in the user list
       * 
       * Every item is a button and has an on press method
       * The selected item has a different style 
       * 
       * @param {*} item item to render
       * 
       * @returns rendered item
       */
      const renderItem = ({ item }) => (
        <TouchableOpacity
          style={[
            styles.item,
            userName === item && styles.itemSelected,
          ]}
          onPress={() => handlePress(item)}
        >
          <Text style={styles.itemText}>{item}</Text>
        </TouchableOpacity>
      );

    /**
     * Return the user custom component
     * 
     * Contain the scollable user list and the user ID
     */
    return (
        <><View style={styles.container}>
          <View>
              <Text style={styles.itemTitle}>Users : </Text>
          </View>
          <View style={styles.containerFlatlist}>
            <FlatList
                horizontal={true}
                data={users}
                renderItem={renderItem}
                keyExtractor={item => item}
                extraData={userName}
            />
            <Text style={styles.text}>
              User ID : {userID ? userID : 'Select a user !'}
            </Text>
          </View> 
        </View>
        </>
  );
};

/**
 * Style sheet containing styles for the component
 */
const styles = StyleSheet.create({
  container: {
      flex: 0.6,
      marginLeft: 15,
      marginRight: 15,
      marginTop: 15,
  }, 
  title: {
      fontSize: 34,
      textDecorationLine: 'underline',
      marginTop: 10,
      marginBottom: 10,
      fontWeight: 'bold',
  },
  text: {
      fontSize: 20,

  },
  containerFlatlist: {
    flex: 1,
  }, 
  item: {
      padding: 10,
      backgroundColor: '#404040',
      marginHorizontal: 5,
      marginTop: 10,
      borderRadius: 10,
      height: 45,
  },
  itemSelected: {
      padding: 10,
      backgroundColor: '#90ad73',
      marginHorizontal: 5,
      marginTop: 10,
      borderRadius: 10,
      height: 45,
  },
  itemText: {
      color: '#fff',
      fontSize: 16,
      marginHorizontal: 15,
  },
  itemTitle: {
      color: '#fff',
      fontSize: 22,
      fontWeight: 'bold',
  },
});

export default User;    // Export the custom component


