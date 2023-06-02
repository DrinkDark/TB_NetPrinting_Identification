# Analyse packet BLE
## Identification with Elatec Mobile Badge BLE
Phone = GATT Server, TWN4 = GATT client

### Phone -> LLCP feature exchange (Link Layer Control Protocol)
-> Return active feature
### TWN4 -> LLCP slave feature exchange 
-> Return active feature
### TWN4 -> ATT exchange MTU transaction
-> MTU = 250, Maximum Transmission Unit (maximum length of an ATT packet)
### Phone -> LLCP data length update
### TWN4 -> LLCP data length update
### Phone -> ATT indication transaction
-> Attribute handle =  3
    Attribute data = 0x0100FFFF
### Phone -> ATT read by type transaction
-> Attribute type = server supported features (handle from 1 to max) 
    Error = Attribute not found
### Phone -> LLCP connection update indication
-> Update connection parameters
### TWN4 -> ATT find by type value transaction
-> Attribute type = Primary service (UUID = 0000B81D-0000-1000-8000-00805F9B34F0) (handle from 1 to max)
    Found attribute handle = 44
### Phone -> ATT read by groupe type transaction
-> Attribute type = Primary service (handle from 1 to max)
    1. Attribute handle = 1, UUID = Generic Attribute
    2. Attribute handle = 9, UUID = Generic Access 
    3. Attribute handle = 14, UUID = Device information
### TWN4 -> ATT read by type transaction
-> Attribute type = Characteristic declaration (handle from 44 to max)
    Characteristic handle = 46, UUID = 0000B81D-0000-1000-8000-00805F9B34F1
### Phone -> ATT read by groupe type transaction
-> Attribute type = Primary service (handle from 23 to max)
    Attribute handle = 23, UUID = 5A44C004-4112-4274-880E-CD9B3DAEDF8E
### Phone -> ATT read by type transaction
-> Attribute type = include (handle from 1 to 8)
   Error = Attribute not found
### TWN4 -> ATT read by type transaction
-> Attribute type = Characteristic Declaration (handle from 46 to max)
   Error = Attribute not found
### TWN4 -> ATT find information transaction
-> (handle from 47 to max)
   Error = Attribute not found
### Phone -> ATT read by type transaction
-> Attribute type = Characteristic Declaration (handle from 1 to 8)

    1. Attribute handle = 3, UUID = Service changed
    2. Attribute handle = 6, UUID = Database Hash
    3. Attribute handle = 8, UUID = Client support features
### Phone -> ATT read by type transaction
-> Attribute type = Characteristic Declaration (handle = 8)
   Error = Attribute not found
### TWN4 -> ATT write command packet
-> (handle = 46), Data = E5 6E 1B 55 A6 49 C6 6B 99 FB 6A 2E E1 1F B0 
62
### Phone -> LLCP version exchange
### Phone -> ATT notification packet
-> (handle = 46), Data = CE CA DD 27 A6 37 48 6B 46 55 84 7C 28 99 E7 
9C
### Phone -> ATT find information transaction
-> (handle = 4)
   Attribute handle = 4, UUID = Client characteristic configuration
### Phone -> ATT read by type transaction
-> Attribute type = include (handle from 9 to 13)
   Error = Attribute not found
### TWN4 -> ATT write command packet
-> (handle = 46), Data = 08 9C 80 8D 5E DD 1C 29 BF E8 38 28 7E 1C E5 
C8
### Phone -> ATT notification packet
-> (handle = 46), Data = 60 1F 9A 6A CA 71 36 31 8E 4D DF 1B D3 C0 AC 
79
### Phone -> ATT read by type transaction
-> Attribute type = Characteristic Declaration (handle = 13)

    1. Attribute handle = 11, UUID = Device name
    2. Attribute handle = 13, UUID = Appearance
### Phone -> ATT read by type transaction
-> Attribute type = Characteristic Declaration (handle from 9 to 13)
   Error = Attribute not found
### TWN4 -> ATT write command packet
-> (handle = 46), Data = 9D 98 48 F9 F3 EA 7E 20 54 4D CE 13 49 12 F6 
13
### Phone -> ATT read by type transaction
-> Attribute type = include (handle from 14 to 22)
   Error = Attribute not found
### Phone -> ATT notification packet
-> (handle = 46), Data = 39 F5 76 42 38 E0 D0 1E 00 00 00 00 00 00 00 
00 (phone ID)