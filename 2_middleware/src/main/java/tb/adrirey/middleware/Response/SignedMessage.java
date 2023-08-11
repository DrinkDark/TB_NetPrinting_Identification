package tb.adrirey.middleware.Response;

/**
 * Signed message class
 */
public class SignedMessage {
    private String signedMessage;

    /**
     * Default constructor
     *
     * @param signedMessage value to store
     */
    public SignedMessage(String signedMessage) {
        this.signedMessage = signedMessage;
    }

    /**
     * signedMessage getter
     *
     * @return signedMessage
     */
    public String getSignedMessage() {
        return signedMessage;
    }

    /**
     * signedMessage setter
     *
     * @param signedMessage value to set
     */
    public void setSignedMessage(String signedMessage) {
        this.signedMessage = signedMessage;
    }
}
