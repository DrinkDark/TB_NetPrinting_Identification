package tb.adrirey.middleware.Response;

/**
 * Encrypted data class
 */
public class EncryptedData {
    private String encryptedData;

    /**
     * Default constructor
     *
     * @param encryptedData value to set
     */
    public EncryptedData(String encryptedData) {
        this.encryptedData = encryptedData;
    }

    /**
     * encryptedData getter
     *
     * @return encryptedData
     */
    public String getEncryptedData() {
        return encryptedData;
    }

    /**
     * encryptedData setter
     *
     * @param encryptedData value to set
     */
    public void setEncryptedData(String encryptedData) {
        this.encryptedData = encryptedData;
    }
}
