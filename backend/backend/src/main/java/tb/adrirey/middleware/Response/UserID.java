package tb.adrirey.middleware.Response;

/**
 * User ID class
 */
public class UserID {
    private String userID;

    /**
     * Default constructor
     *
     * @param userID value to set
     */
    public UserID(String userID) {
        this.userID = userID;
    }

    /**
     * userID getter
     *
     * @return userID
     */
    public String getUserID() {
        return userID;
    }

    /**
     * userID setter
     * @param userID value to set
     */
    public void setUserID(String userID) {
        this.userID = userID;
    }
}
