import { useState } from "react";
import { PermissionsAndroid, Platform} from "react-native";
import { BleManager, Device } from "react-native-ble-plx";
 

type PermissionCallback = (result: boolean) => void;

const bleManager = new BleManager();

interface BluetoothlowEnergyApi {
    requestPermissions(callback: PermissionCallback): Promise<void>;
    scanForDevices(): void;
    allDevices: Device[];
}

export default function useBle(): BluetoothlowEnergyApi{
//const [allDevices, setAllDevices] = useState<Device[]>([]);

    const requestPermissions = async (callback: PermissionCallback) => {
        const grantedStatus = await PermissionsAndroid.request(
            PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
            {
                title: 'Location Permission',
                message: 'Bluetooth Low Energy Needs Location Permission',
                buttonNegative: 'Cancel',
                buttonPositive: 'Ok',
                buttonNeutral: "Maybe Later"
            },
        );
        callback(grantedStatus === PermissionsAndroid.RESULTS.GRANTED);
    }

    const isDuplicateDevice = (devices: Device[], nextDevice: Device) => 
        devices.findIndex(device => nextDevice.id === device.id) > -1;

    const scanForDevices = () => {
        bleManager.startDeviceScan(null, null, (error, device) => {
            if(error) {
                console.log(JSON.stringify(error));
            } else if (device != null){
                console.log(device.name);
            }
            
            /*if(device && device.name?.includes('ELATEC')){
                setAllDevices(prevState => {
                    if (!isDuplicateDevice(prevState, device)) {
                        return[...prevState, device]
                    }
                    return prevState;
                });
            }*/
        });
    };

    return {
        requestPermissions,
        scanForDevices
        //allDevices
    } ;
}

function requestMultiple(arg0: any[]): any {
    throw new Error("Function not implemented.");
}
