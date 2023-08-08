///////////////////////////////////////////////////
//             UseUser custom hook
///////////////////////////////////////////////////

import { useState, useEffect } from 'react';

import axios from 'axios';

import BLE from '../components/BLE';

const ipAddress = '192.168.137.1';        // Middleware component address

 
/**
 * Use user custom hook constructor
 * 
 * @returns return values
 */
const useUser = () => {
  const [userName, setUserName] = useState('');   // Initialize user name useState hook
  const [userID, setUserID] = useState('');       // Initialize user ID useState hook
  const [users, setUsers] = useState([]);         // Initialize user useState hook

  /**
   * Add new user to the hook
   * 
   * @param userList user list to add
   */
  const addUsers = (userList) => {
    setUsers(userList);
  };
  



  /**
   * Use effect define
   * 
   * Get the first time the user list and when a new user is selected, the user identifier is retrieve
   * Called every time a the user name changed
   */
  useEffect(() => {
    // If no user is selected, the user list is retrieve from the middleware component
    if(users && !userName){
      axios.get(`http://${ipAddress}:8080/getUserList`)
      .then(response => {
        addUsers(response.data.userList);
        console.log('UserList : ' + response.data.userList);
      })
      .catch(error => {
        console.error(error);
      });
    }

     // If a user is selected, the user identifier is retrieve from the middleware component
    if(userName){
      axios.get(`http://${ipAddress}:8080/getUserID?userName=${userName}`)
      .then(response => {
        setUserID(response.data.userID)
        BLE.setUserID(response.data.userID);    // Set user ID on the BLE component
        console.log('UserID : ' + response.data.userID);
      })
      .catch(error => {
        console.error(error);
      });
      
    }
  },[userName]);    // Every time the username change, the useEffect is called

  // 
  /**
   * User name changed
   * 
   * @param value changed value to updated
   */
  const onChangeUserName = (value) => {
    setUserName(value);
  };

  return [userName, onChangeUserName, userID, users];
};

export default useUser;   // Export custom useUser hook


