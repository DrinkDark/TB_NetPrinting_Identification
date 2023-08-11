package tb.adrirey.middleware.Response;

/**
 * User exists class
 */
public class UserExists {
    private Boolean userExists;

    /**
     * Default constructor
     *
     * @param userExists value to set
     */
    public UserExists(Boolean userExists) {
        this.userExists = userExists;
    }

    /**
     * userExists getter
     *
     * @return userExists
     */
    public Boolean getUserExists() {
        return userExists;
    }

    /**
     * userExists setter
     *
     * @param userExists value to set
     */
    public void setUserExists(Boolean userExists) {
        this.userExists = userExists;
    }
}
