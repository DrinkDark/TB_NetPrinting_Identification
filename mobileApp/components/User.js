import React from 'react';

import {
    StyleSheet,
    Text,
    FlatList,
    View,
    TouchableOpacity,
  } from 'react-native';

import useUser from '../hooks/useUser'

const User = () => {
    const [userName, onChangeUserName, userID, users] = useUser();

    const Item = ({ title }) => (
        <Pressable
          style={[
            styles.itemSelected,
          ]}
          onPress={() => onChangeUserName(title)}
        >
          <Text style={styles.itemText}>{title}</Text>
        </Pressable>
      );

      const handleUserPress = (user) => {
        if (userName === user) {
            onChangeUserName(null);
        } else {
            onChangeUserName(user);
        }
      };
    
      const renderItem = ({ item }) => (
        <TouchableOpacity
          style={[
            styles.item,
            userName === item && styles.itemSelected,
          ]}
          onPress={() => handleUserPress(item)}
        >
          <Text style={styles.itemText}>{item}</Text>
        </TouchableOpacity>
      );

      
    /*const Item = ({title}) => (
        <View style={styles.item}>
        <Text style={styles.itemText}>{title}</Text>
        </View>
    );*/

    return (
        <><View style={styles.container}>
            <View>
                <Text style={styles.itemTitle}>Users : </Text>
            </View>
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
        </>
  );
};


const styles = StyleSheet.create({
    container: {
        flex: 0.28,
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
        marginBottom: 6,
        marginTop: 10,
    },
    item: {
        padding: 10,
        backgroundColor: '#404040',
        marginHorizontal: 5,
        marginTop: 10,
        borderRadius: 10,
    },
    itemSelected: {
        padding: 10,
        backgroundColor: '#90ad73',
        marginHorizontal: 5,
        marginTop: 10,
        borderRadius: 10,
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

export default User;


