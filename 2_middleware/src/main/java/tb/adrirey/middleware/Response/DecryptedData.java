package tb.adrirey.middleware.Response;

/**
 * Decrypted data class
 */
public class DecryptedData {
    private String decryptedData;

    /**
     * Default constructor
     *
     * @param decryptedData value to set
     */
    public DecryptedData(String decryptedData) {
        this.decryptedData = decryptedData;
    }

    /**
     * decryptedData getter
     *
     * @return decryptedData
     */
    public String getDecryptedData() {
        return decryptedData;
    }

    /**
     * decryptedData setter
     *
     * @param decryptedData value to set
     */
    public void setDecryptedData(String decryptedData) {
        this.decryptedData = decryptedData;
    }
}
