# NetPrinting Wireless Identification Bachelor Thesis

## Introduction
The Swiss company Polyright provides centralised identification, access and payment systems using RFID cards. Future developments will focus on the digitalisation of RFID cards. 
Users will be able to use their smartphone instead of a card. In most cases a QR code will be used. Both the smartphone and the device will be connected via a cloud. The presence of the phone would therefore be reported to the device using the latter.

However, this solution is not possible for printers. Each Polyright's customer uses the brand and model of printer he prefers. It would be necessary to adapt the program for each new device model to enable communication with the cloud.

This bachelor thesis aims to develop a new authentication solution:
- Based on a existing RFID identification solution.
- Using Bluetooth Low Energy (BLE) communication with a smartphone.

This new authentication system has to be secure to prevent any kind of attack.

## Objectives
1. Analysis of Polyright's existing identitication system and ecosystem.
2. Setting up a test bed to simulate a real situation.
3. Analysis of the security aspects and specification of the authentication protocol.
4. Programming the RFID/BLE reader with the authentication protocol.
5. Analysis of existing cross-platform mobile application frameworks (iOs, An-droid) and development of an authenticated smartphone test application.

## Content
### **1. Documentation**
All the diagrams and the BLE analyse done using a BLE sniffer

### **2. Middleware**
Web service coded in Java using the Spring framework. Communicates with the Print manager serveur via a XML API to retrieve usefull informations. Provides a REST service for the mobile application to get print server data, encrypted and decrypted data and the signed message for the authentication.

| **Tools** | **Version** | **Website** |
|----------|----------|----------|
| IntelliJ | 2021.3 | https://www.jetbrains.com/idea/download/ |
| Apache Maven | 3.9.2 | https://maven.apache.org/download.cgi |

| **Libraries** | **Version** | **Website** |
|----------|----------|----------|
| xmlrpc | 2.0 | https://mvnrepository.com/artifact/xmlrpc/xmlrpc |
| commons-codec | 1.3 | https://mvnrepository.com/artifact/commons-codec/commons-codec/1.3 |
        
### **3. PaperCut manager**
Java application with a Swing GUI to modify data in the Print Manager Server using the XML API. Can add user, add users from a list, delete user, delete all users and get user balance.

| **Tools** | **Version** | **Website** |
|----------|----------|----------|
| IntelliJ | 2021.3 | https://www.jetbrains.com/idea/download/ |
| Apache Maven | 3.9.2 | https://maven.apache.org/download.cgi |

| **Libraries** | **Version** | **Website** |
|----------|----------|----------|
| xmlrpc | 2.0 | https://mvnrepository.com/artifact/xmlrpc/xmlrpc |
| commons-codec | 1.3 | https://mvnrepository.com/artifact/commons-codec/commons-codec/1.3 |

### **4. Card reader**
Firmware coded in C for the *Elatec TWN4 slim card* reader. Can performed authentification using BLE with the mobile application. The developement pack is also include.

| **Tools** | **Version** | **Website** |
|----------|----------|----------|
| Visual Studio Code | - | https://code.visualstudio.com/ |
| TWN4 DevPack | 4.51 | https://www.elatec-rfid.com/int/elatec-software |

### **5. Mobile application**
The test mobile application in React Native. Retrieve the user list from the Middleware using the provided Rest service. Can discover device with the specific service UUID. Connect to them by approching the smartphone and then start the authentication protocol. If succeed, it transmit the sigend message containing the selecteed user ID with the current time and the message expiration time.

| **Tools** | **Version** | **Website** |
|----------|----------|----------|
| Visual Studio Code | - | https://code.visualstudio.com/ |
| Node.js | 18.61.1 | https://nodejs.org/fr |
| npm | 9.5.1 | include in Node.js |

| **Libraries** | **Version** | **Website** |
|----------|----------|----------|
| xmlrpc | 2.0 | https://mvnrepository.com/artifact/xmlrpc/xmlrpc |
| commons-codec | 1.3 | https://mvnrepository.com/artifact/commons-codec/commons-codec/1.3 |
| react | 18.2.0 | https://react.dev/learn/installation |
| react-native | 0.72.0 | https://reactnative.dev/docs/environment-setup |
| react-native-ble-plx | 2.0.3 | https://www.npmjs.com/package/react-native-ble-plx |
| react-native-base64 | 0.2.1 | https://www.npmjs.com/package/react-native-base64 |
| react-native-permissions | 3.8.1 | https://www.npmjs.com/package/react-native-permissions |
| axios | 1.4.0 | https://www.npmjs.com/package/axios |
| buffer | 6.0.3 | https://www.npmjs.com/package/buffer |

### **6. Print Release Station P**I
Raspberry PI image for the Print Release Station.

## Hardware used
- ElatecTWN4 slim card reader (Bootloader V1.06)
- Google Pixel 3 smartphone (mode developer)
- ZyXEL ES-105A v3 switch
- Ellysis Vanguard Bluetooth sniffer
- HP LaserJet P3015 printer

## Authors and contributors :
* **Adrien Rey** - *Student*
* **Christopher Métrailler** - *Professor*
* **Christophe Pierroz** - *Expert*
