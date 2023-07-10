import React from 'react';

import {
    StyleSheet,
    Text,
    FlatList,
    View,
    Pressable,
  } from 'react-native';

import useUser from '../hooks/useUser'

const User = () => {
    const [userName, userID, users] = useUser();

    const Item = ({title}) => (
        <View style={styles.item}>
        <Text style={styles.itemText}>{title}</Text>
        </View>
    );

    return (
        <><View style={styles.container}>
            <View>
                <Text style={styles.itemTitle}>Users : </Text>
            </View>
            <FlatList
                horizontal={true}
                data={users}
                renderItem={({item}) => <Item title={item} />}
                keyExtractor={item => item}
        
            />
        </View>
        </>
  );
};


const styles = StyleSheet.create({
    container: {
        flex: 0.17,
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
        fontSize: 24,
        marginBottom: 6,
    },
    item: {
        padding: 10,
        backgroundColor: '#404040',
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


