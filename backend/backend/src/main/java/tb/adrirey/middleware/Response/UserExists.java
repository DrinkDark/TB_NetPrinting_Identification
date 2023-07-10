package tb.adrirey.middleware.Response;

public class UserExists {
    private Boolean userExists;

    public UserExists(Boolean userExists) {
        this.userExists = userExists;
    }

    public Boolean getUserExists() {
        return userExists;
    }

    public void setUserExists(Boolean userExists) {
        this.userExists = userExists;
    }
}
