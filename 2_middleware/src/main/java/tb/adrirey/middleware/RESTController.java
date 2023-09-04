package tb.adrirey.middleware;

import org.apache.commons.codec.DecoderException;
import org.apache.commons.codec.binary.Hex;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import tb.adrirey.middleware.Response.*;

import java.security.SecureRandom;
import java.time.Instant;

import static org.apache.tomcat.util.buf.HexUtils.toHexString;

/**
 * RestController
 *
 * Spring object acting as a REST service
 * Also communicate via XML API to the print manager server
 */
@RestController
public class RESTController {
    private ServerCommandProxy scp; // Proxy for the print manager server communication
    private Security sec;           // Security object
    
    /**
     * Default constructor
     *
     * Initialize the ServerCommandProxy and Security object
     */
    public RESTController() {
        scp = new ServerCommandProxy("PaperCutServer", 9191, "authToken");  // Open XML proxy with server
        sec = new Security();
    }

    /**
     * Transform long variable to byte array (length 8)
     *
     * @param value variable to transform
     * @return transformed variable
     */
    public byte[] longTo8ByteArray(long value) {
        return new byte[]{
                (byte) (value >>> 56),
                (byte) (value >>> 48),
                (byte) (value >>> 40),
                (byte) (value >>> 32),
                (byte) (value >>> 24),
                (byte) (value >>> 16),
                (byte) (value >>> 8),
                (byte) value
        };
    }

    /**
     * Get request for user name
     *
     * Handles a GET request to fetch a user ID based on their username.
     * Return it in JSON format
     *
     * @param userName userName to get userID
     * @return ResponseEntity containing the user ID
     */
    @RequestMapping(method = RequestMethod.GET, path ="/getUserID")
    public ResponseEntity<UserID> getUserID(@RequestParam String userName) {
        UserID response = new UserID(scp.getUserProperty(userName, "secondary-card-number"));   // Get secondary card number (= user ID) from papercut server
        return ResponseEntity.ok(response);
    }

    /**
     * Get request to user list
     *
     * Handles a GET request to fetch the user list.
     * Return it in JSON format
     *
     * @return ResponseEntity containing the user list
     */
    @RequestMapping(method = RequestMethod.GET, path ="/getUserList")
    public ResponseEntity<UserList> getUserList() {
        UserList response = new UserList(scp.listUserAccounts(0,1000));     // Get the user list from papercut server
        return ResponseEntity.ok(response);
    }

    /**
     * Get method to verify the existence of a user
     *
     * Handles a GET request check if a user exist in the PaperCut server based on their username.
     * Return it in JSON format
     *
     * @param userName user to verify existence
     * @return ResponseEntity containing true if exists, else false
     */
    @RequestMapping(method = RequestMethod.GET, path ="/userExists")
    public ResponseEntity<UserExists> userExists(@RequestParam String userName) {
        UserExists response = new UserExists(scp.isUserExists(userName));    //Check if a user exist in the PaperCut server
        return ResponseEntity.ok(response);
    }

    /**
     * Get method for the user balance
     *
     * Handles a GET request fetch the balance based on their username.
     * Return it in JSON format
     *
     * @param userName userName to get balance
     * @return ResponseEntity containing the balance
     */
    @RequestMapping(method = RequestMethod.GET, path = "/getUserBalance")
    public ResponseEntity<UserBalance> getUserBalance(@RequestParam String userName) {
        UserBalance response = new UserBalance(scp.getUserAccountBalance(userName));    //Get user credit from papercut server
        return ResponseEntity.ok(response);
    }

    /**
     * Get method for encrypt data
     *
     * Handles a GET request to encrypt data
     * Return it in JSON format
     *
     * @param data data to encrypt
     * @return  ResponseEntity containing the encrypted data
     */
    @RequestMapping(method = RequestMethod.GET, path ="/getEncryptData")
    public ResponseEntity<EncryptedData> aesEncryptedData(@RequestParam String data) {
        EncryptedData response = new EncryptedData(sec.encryptData(data));      // Encrypt using the Security class
        return ResponseEntity.ok(response);
    }

    /**
     * Get method for decrypt data
     *
     * Handles a GET request to decrypt data
     * Return it in JSON format
     *
     * @param data data to decrypt
     * @return  ResponseEntity containing the decrypted data
     */
    @RequestMapping(method = RequestMethod.GET, path ="/getDecryptData")
    public ResponseEntity<DecryptedData> aesDecryptedData(@RequestParam String data) {
        DecryptedData response = new DecryptedData(sec.decryptData(data));      // Decrypt using the Security class
        return ResponseEntity.ok(response);
    }


    /**
     * Get method for a random 16bytes number
     *
     * Handles a GET request to generate a 16 bytes number
     * Return it in JSON format
     *
     * @return ResponseEntity containing the random number
     */
    @RequestMapping(method = RequestMethod.GET, path ="/getRandNum")
    public ResponseEntity<RandomNumber> getRandNum() {
        SecureRandom sr = new SecureRandom();
        byte[] rndBytes = new byte[16];

        sr.nextBytes(rndBytes);

        RandomNumber response = new RandomNumber(toHexString(rndBytes));      //Convert the byte array to hex string
        return ResponseEntity.ok(response);
    }

    /**
     * Get method for the signed message
     *
     * Handles a GET request to generate the signed message.
     * The message is an encrypted 32 bytes array containing the userID the current time and the expiration time in unix timestamp
     *
     * Message structure :
     *
     * || 31 30 29 28 27 26 25 24 || 23 22 21 20 19 18 17 16 || 15 14 13 12 11 10  9  8 || 7  6  5  4  3  2  1  0 ||
     *              ^                        ^                          ^                            ^
     *        8 bytes = userID     8 bytes = current time       8 bytes = current time       8 bytes of padding = 0x00000000
     *
     * Return it in JSON format
     *
     * @param userID userName to get userID
     * @return  ResponseEntity containing the signed message
     */
    @RequestMapping(method = RequestMethod.GET, path ="/getSignedMessage")
    public ResponseEntity<SignedMessage> getSignedMessage(@RequestParam String userID) throws DecoderException {
        int validityTime = 24 * 3600;
        byte signedMessage[] = new byte[32];
        byte userIDArray[] = new byte[8];
        userIDArray = Hex.decodeHex(userID.toCharArray());

        System.arraycopy(userIDArray, 0, signedMessage, (8 - Math.min(userIDArray.length, 8)), Math.min(userIDArray.length, 8));                                    //Copy the user into the signed message (maximum 8 bytes, if more take 8 first bytes)
        System.arraycopy(longTo8ByteArray(Instant.now().getEpochSecond()), 0, signedMessage, 8, 8);                            // Copy the current time into the signed message
        System.arraycopy(longTo8ByteArray(Instant.now().getEpochSecond() + validityTime) , 0, signedMessage, 16, 8);        // Copy the expiration time into the signed message (24h from current time)

        SignedMessage response = new SignedMessage(toHexString(signedMessage));    ;      //Convert the byte array to hex string
        return ResponseEntity.ok(response);
    }
}
