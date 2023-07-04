package tb.adrirey.backend;

import org.apache.commons.codec.DecoderException;
import org.apache.commons.codec.binary.Hex;
import org.springframework.web.bind.annotation.*;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.SecretKeySpec;
import java.nio.charset.StandardCharsets;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.NoSuchAlgorithmException;

import static org.apache.tomcat.util.buf.HexUtils.toHexString;

@CrossOrigin(origins = "http://localhost:19006", maxAge = 3600)
@RestController
public class RESTController {
    ServerCommandProxy scp;


    private byte[] key = {(byte) 0xbf, (byte) 0xc1, (byte) 0xc1, (byte) 0x8b, (byte) 0x3c, (byte) 0x60, (byte) 0x50, (byte) 0x2a,
                          (byte) 0x4f, (byte) 0x08, (byte) 0xdf, (byte) 0xb6, (byte) 0xe0, (byte) 0xd9, (byte) 0xd1, (byte) 0x1f};
    private final Key aesMasterKey = new SecretKeySpec(key, "AES");

    public RESTController() {
        scp = new ServerCommandProxy("PaperCutServer", 9191, "authToken");
    }

    //Get secondary card number (= user ID) from papercut server
    @RequestMapping(method = RequestMethod.GET, path ="/getUserID")
    public String getUserID(@RequestParam String data) {
        return scp.getUserProperty(data, "secondary-card-number");
    }

    //Check if a user exist
    @RequestMapping(method = RequestMethod.GET, path ="/userExist")
    public Boolean userExist(@RequestParam String data) {
        return scp.isUserExists(data);
    }

    //Get credit from papercut server
    @RequestMapping(method = RequestMethod.GET, path = "/getCredit")
    public double getCredit(@RequestParam String data) {
        return scp.getUserAccountBalance(scp.lookUpUserNameByCardNo(data));
    }

    @RequestMapping(method = RequestMethod.GET, path ="/getEncryptData")
    public String aesEncryptedData(@RequestParam String data) throws NoSuchPaddingException, IllegalBlockSizeException, NoSuchAlgorithmException, BadPaddingException, InvalidKeyException, DecoderException {
        Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
        cipher.init(Cipher.ENCRYPT_MODE, aesMasterKey);
        byte plainText[] = Hex.decodeHex(data.toCharArray());
        byte cipherText[] =  cipher.doFinal(plainText);
        return toHexString(cipherText);
    }

    @RequestMapping(method = RequestMethod.GET, path ="/getDecryptData")
    public String aesDecryptedData(@RequestParam String data) throws NoSuchPaddingException, IllegalBlockSizeException, NoSuchAlgorithmException, BadPaddingException, InvalidKeyException, DecoderException {
        Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
        cipher.init(Cipher.DECRYPT_MODE, aesMasterKey);
        byte plainText[] = Hex.decodeHex(data.toCharArray());
        byte cipherText[] =  cipher.doFinal(plainText);
        return toHexString(cipherText);
    }
}
