package tb.adrirey.backend;
import javax.crypto.*;
import javax.crypto.spec.SecretKeySpec;
import java.nio.charset.StandardCharsets;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;

public class User {
    private int userID = 1756;
    private String name = "Didier";
    private String surname = "Merlot";
    private float credit = 10;

    private byte[] key = {(byte) 0xbf, (byte) 0xc1, (byte) 0xc1, (byte) 0x8b, (byte) 0x3c, (byte) 0x60, (byte) 0x50, (byte) 0x2a,
            (byte) 0x4f, (byte) 0x08, (byte) 0xdf, (byte) 0xb6, (byte) 0xe0, (byte) 0xd9, (byte) 0xd1, (byte) 0x1f};
    private final Key aesMasterKey = new SecretKeySpec(key, "AES");

    public String getName() {
        return name;
    }

    public String getSurname() {
        return surname;
    }

    public float getCredit() {
        return credit;
    }

    public int getUserID() {
        return userID;
    }

    public String aesEncryptedData(String data) throws NoSuchPaddingException, NoSuchAlgorithmException, InvalidKeyException, IllegalBlockSizeException, BadPaddingException {
        Cipher cipher = Cipher.getInstance("AES");
        cipher.init(Cipher.ENCRYPT_MODE, aesMasterKey);
        byte[] cipherText = cipher.doFinal(data.getBytes(StandardCharsets.UTF_8));
        return Base64.getEncoder().encodeToString(cipherText);
    }

}
