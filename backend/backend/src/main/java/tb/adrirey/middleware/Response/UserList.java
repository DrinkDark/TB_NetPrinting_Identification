package tb.adrirey.middleware.Response;

import java.util.Vector;

public class UserList {
    private Vector<String> userList;

    public UserList(Vector<String> userList) {
        this.userList = userList;
    }

    public Vector<String> getUserList() {
        return userList;
    }

    public void setUserList(Vector<String> userList) {
        this.userList = userList;
    }
}
