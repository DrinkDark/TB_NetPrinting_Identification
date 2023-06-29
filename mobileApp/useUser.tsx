import { useState, useEffect } from 'react';
import axios from 'axios';
const ipAddress = '10.93.11.8';

const useUser = () => {
  const [userName, setUserName] = useState('');
  const [userID, setUserID] = useState('');

  useEffect(() => {
    if (userName !== '') {
      axios.get(`http://${ipAddress}:8080/userExist?data=${userName}`)
        .then(response => {
          if(response.data){
            axios.get(`http://${ipAddress}:8080/getUserID?data=${userName}`)
            .then(response => {
              const userID = response.data;
              setUserID(userID);
              console.log('UserID: ' + userID);
            })
            .catch(error => {
              console.error(error);
            });
          } else {
            setUserID('');
          }
        });
    }
  }, [userName]);

  const onChangeUserName = (value) => {
    setUserName(value);
  };

  return [userName, onChangeUserName, userID];
};

export default useUser;
