import { useState, useEffect } from 'react';
import axios from 'axios';

const ipAddress = '10.93.9.38'; //network card wifi

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
    if(users){
      axios.get(`http://${ipAddress}:8080/getUserList`)
      .then(response => {
        addUsers(response.data.userList);
        console.log('UserList : ' + response.data.userList);
      })
      .catch(error => {
        console.error(error);
      });
    }
  },[]);

  //setUserID(response.data.userID);
  //BLE.setUserID(response.data.userID);

  const onChangeUserName = (value) => {
    setUserName(value);
  };

  return [userName, userID, users];
};

export default useUser;


