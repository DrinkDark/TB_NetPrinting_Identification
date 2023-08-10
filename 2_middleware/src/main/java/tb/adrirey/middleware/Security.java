package tb.adrirey.middleware;
import org.apache.commons.codec.DecoderException;
import org.apache.commons.codec.binary.Hex;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.SecretKeySpec;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.NoSuchAlgorithmException;

import static org.apache.tomcat.util.buf.HexUtils.toHexString;

/**
 * Security class
 *
 * Encrypt and decrypt data using Cipher from java.
 * Using AES encryption with ECB and no padding.
 */
public class Security {

    private byte[] key = {(byte) 0xbf, (byte) 0xc1, (byte) 0xc1, (byte) 0x8b, (byte) 0x3c, (byte) 0x60, (byte) 0x50, (byte) 0x2a,
            (byte) 0x4f, (byte) 0x08, (byte) 0xdf, (byte) 0xb6, (byte) 0xe0, (byte) 0xd9, (byte) 0xd1, (byte) 0x1f};

    private Cipher cipher;
    private String transformation = "AES/ECB/NoPadding";
    private Key aesKey;

    /**
     * Default constructor
     *
     * Create cipher and the aes key. Handle teh exception for the cipher creation
     */
    Security() {
        try {
            // Check if AES encryption with a key size > 128 bits is supported
            if (Cipher.getMaxAllowedKeyLength("AES") < 128) {
                throw new NoSuchAlgorithmException("AES encryption with key size > 128 bits is not supported.");
            }

            cipher = Cipher.getInstance(transformation);
            aesKey = new SecretKeySpec(key, "AES");

        } catch (NoSuchPaddingException | NoSuchAlgorithmException ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Encrypt data method
     *
     * Encrypt using the set transformation and return the value in a Hex string
     * Handle the possible exceptions.
     *
     * @param data data to encrypt
     * @return  encrypted data in hex string
     */
    public String encryptData(String data)  {
        try {
            cipher.init(Cipher.ENCRYPT_MODE, aesKey);               // Init cipher in encrypt mode with the correct key
            byte plainText[] = Hex.decodeHex(data.toCharArray());   // Transform received Hex string into a byte array
            byte cipherText[] = cipher.doFinal(plainText);          // Encrypt data
            return toHexString(cipherText);                         // Transform encrypt data in hex string and return it

        } catch (InvalidKeyException | DecoderException | IllegalBlockSizeException | BadPaddingException ex) {
            ex.printStackTrace();
            return null;
        }
    }

    /**
     * Decrypt data method
     *
     * Decrypt using the set transformation and return the value in a Hex string
     * Handle the possible exceptions.
     *
     * @param data data to encrypt
     * @return  encrypted data in hex string
     */
    public String decryptData(String data) {
        try {
            cipher.init(Cipher.DECRYPT_MODE, aesKey);               // Init cipher in decrypt mode with the correct key
            byte cipherText[] = Hex.decodeHex(data.toCharArray());  // Transform received Hex string into a byte array
            byte plainText[] =  cipher.doFinal(cipherText);         // Decrypt data
            return toHexString(plainText);                          // Transform decrypt data in hex string and return it

        } catch (InvalidKeyException | DecoderException | IllegalBlockSizeException | BadPaddingException ex) {
            ex.printStackTrace();
            return null;
        }
    }

}
