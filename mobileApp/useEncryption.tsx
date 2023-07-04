import { useState, useEffect } from 'react';
import axios from 'axios';
const ipAddress = '10.93.11.8';

const useEncryption = () => {
  const [plainText, setPlainText] = useState('');
  const [cipherText, setCipherText] = useState('');

  useEffect(() => {
    setPlainText(plainText);
    setCipherText(cipherText);
  }, [plainText, cipherText]);

  const encryptData = (value) => {
    setPlainText(value);
    if (plainText.length === 32) {
      axios.get(`http://${ipAddress}:8080/getEncryptData?data=${plainText}`)
      .then(response => {
        setCipherText(response.data);
        console.log('Cipher text : ' + response.data);
      })
      .catch(error => {
        console.error(error);
      });
    }
  };

  const decryptData = (value) => {
    setCipherText(value);
    if (cipherText.length === 32) {
      axios.get(`http://${ipAddress}:8080/getDecryptData?data=${cipherText}`)
      .then(response => {
        setPlainText((response.data));
        console.log('Plain text : ' + response.data);
      })
      .catch(error => {
        console.error(error);
      });
    }
  };

  return [plainText, cipherText,encryptData, decryptData];
};

export default useEncryption;


