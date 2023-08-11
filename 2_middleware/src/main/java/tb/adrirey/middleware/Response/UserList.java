package tb.adrirey.middleware.Response;

import java.util.Vector;

/**
 * User list class
 */
public class UserList {
    private Vector<String> userList;

    /**
     * Default constructor
     *
     * @param userList value to set
     */
    public UserList(Vector<String> userList) {
        this.userList = userList;
    }

    /**
     * userList getter
     *
     * @return userList
     */
    public Vector<String> getUserList() {
        return userList;
    }

    /**
     * userList setter
     * @param userList value to set
     */
    public void setUserList(Vector<String> userList) {
        this.userList = userList;
    }
}
