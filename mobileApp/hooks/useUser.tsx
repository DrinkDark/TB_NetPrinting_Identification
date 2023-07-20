import { useState, useEffect } from 'react';
import axios from 'axios';

import BLE from '../components/BLE';

const ipAddress = '192.168.137.1'; //network card wifi

const useUser = () => {
  const [userName, setUserName] = useState('');
  const [userID, setUserID] = useState('');
  const [users, setUsers] = useState([]);

  const addUsers = (userList) => {
    setUsers(userList);
  };
  
  const clearUsers = () => {
      setUsers([]);
  };

  useEffect(() => {
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
    if(userName){
      axios.get(`http://${ipAddress}:8080/getUserID?userName=${userName}`)
      .then(response => {
        setUserID(response.data.userID)
        BLE.setUserID(response.data.userID);
        console.log('UserID : ' + response.data.userID);
      })
      .catch(error => {
        console.error(error);
      });
      
    }
  },[userName]);

  const onChangeUserName = (value) => {
    setUserName(value);
  };

  return [userName, onChangeUserName, userID, users];
};

export default useUser;


