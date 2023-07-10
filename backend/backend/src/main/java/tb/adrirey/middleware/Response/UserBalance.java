package tb.adrirey.middleware.Response;

public class UserBalance {
    private double userBalance;

    public UserBalance(double userBalance) {
        this.userBalance = userBalance;
    }

    public double getUserBalance() {
        return userBalance;
    }

    public void setUserBalance(double userBalance) {
        this.userBalance = userBalance;
    }
}
