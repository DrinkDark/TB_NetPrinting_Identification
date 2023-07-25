package tb.adrirey.middleware;

import org.apache.commons.codec.DecoderException;
import org.apache.commons.codec.binary.Hex;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import tb.adrirey.middleware.Response.*;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.SecretKeySpec;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import static org.apache.tomcat.util.buf.HexUtils.toHexString;

@RestController
public class RESTController {
    private ServerCommandProxy scp;

    private byte[] key = {(byte) 0xbf, (byte) 0xc1, (byte) 0xc1, (byte) 0x8b, (byte) 0x3c, (byte) 0x60, (byte) 0x50, (byte) 0x2a,
                          (byte) 0x4f, (byte) 0x08, (byte) 0xdf, (byte) 0xb6, (byte) 0xe0, (byte) 0xd9, (byte) 0xd1, (byte) 0x1f};

    private final Key aesMasterKey = new SecretKeySpec(key, "AES");

    public RESTController() {
        scp = new ServerCommandProxy("PaperCutServer", 9191, "authToken");
    }

    //Get secondary card number (= user ID) from papercut server
    @RequestMapping(method = RequestMethod.GET, path ="/getUserID")
    public ResponseEntity<UserID> getUserID(@RequestParam String userName) {
        UserID response = new UserID(scp.getUserProperty(userName, "secondary-card-number"));
        return ResponseEntity.ok(response);
    }

    //Get user list from papercut server
    @RequestMapping(method = RequestMethod.GET, path ="/getUserList")
    public ResponseEntity<UserList> getUserList() {
        UserList response = new UserList(scp.listUserAccounts(0,1000));
        return ResponseEntity.ok(response);
    }

    //Check if a user exist in the PaperCut server
    @RequestMapping(method = RequestMethod.GET, path ="/userExists")
    public ResponseEntity<UserExists> userExists(@RequestParam String userName) {
        UserExists response = new UserExists(scp.isUserExists(userName));
        return ResponseEntity.ok(response);
    }

    //Get user credit from papercut server
    @RequestMapping(method = RequestMethod.GET, path = "/getUserBalance")
    public ResponseEntity<UserBalance> getUserBalance(@RequestParam String userName) {
        UserBalance response = new UserBalance(scp.getUserAccountBalance(userName));
        return ResponseEntity.ok(response);
    }

    //Encrypt received data using AES-128 (ECB, no padding)
    @RequestMapping(method = RequestMethod.GET, path ="/getEncryptData")
    public ResponseEntity<EncryptedData> aesEncryptedData(@RequestParam String data) throws NoSuchPaddingException, IllegalBlockSizeException, NoSuchAlgorithmException, BadPaddingException, InvalidKeyException, DecoderException {
        Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
        cipher.init(Cipher.ENCRYPT_MODE, aesMasterKey);
        byte plainText[] = Hex.decodeHex(data.toCharArray());
        byte cipherText[] =  cipher.doFinal(plainText);
        EncryptedData response = new EncryptedData(toHexString(cipherText));
        return ResponseEntity.ok(response);
    }

    //Decrypt received data using AES-128 (ECB, no padding)
    @RequestMapping(method = RequestMethod.GET, path ="/getDecryptData")
    public ResponseEntity<DecryptedData> aesDecryptedData(@RequestParam String data) throws NoSuchPaddingException, IllegalBlockSizeException, NoSuchAlgorithmException, BadPaddingException, InvalidKeyException, DecoderException {
        Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
        cipher.init(Cipher.DECRYPT_MODE, aesMasterKey);
        byte plainText[] = Hex.decodeHex(data.toCharArray());
        byte cipherText[] =  cipher.doFinal(plainText);
        DecryptedData response = new DecryptedData(toHexString(cipherText));
        return ResponseEntity.ok(response);
    }

    //get a random 16bytes number
    @RequestMapping(method = RequestMethod.GET, path ="/getRandNum")
    public ResponseEntity<RandNum> getRandNum() {
        SecureRandom sr = new SecureRandom();
        byte[] rndBytes = new byte[16];

        sr.nextBytes(rndBytes);

        RandNum response = new RandNum(toHexString(rndBytes));
        return ResponseEntity.ok(response);
    }
}
