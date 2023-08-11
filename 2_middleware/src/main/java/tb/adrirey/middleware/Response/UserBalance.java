package tb.adrirey.middleware.Response;

/**
 * User balance class
 */
public class UserBalance {
    private double userBalance;

    /**
     * Default constructor
     *
     * @param userBalance value to set
     */
    public UserBalance(double userBalance) {
        this.userBalance = userBalance;
    }

    /**
     * userBalance getter
     *
     * @return userBalance
     */
    public double getUserBalance() {
        return userBalance;
    }

    /**
     * userBalance setter
     * @param userBalance value to set
     */
    public void setUserBalance(double userBalance) {
        this.userBalance = userBalance;
    }
}
