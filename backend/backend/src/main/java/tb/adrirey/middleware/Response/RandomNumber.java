package tb.adrirey.middleware.Response;

/**
 * Random number class
 */
public class RandomNumber {
    private String randomNumber;

    /**
     * Default constructor
     *
     * @param randomNumber value to set
     */
    public RandomNumber(String randomNumber) {
        this.randomNumber = randomNumber;
    }

    /**
     * randNum getter
     *
     * @return randomNumber
     */
    public String getRandNum() {
        return randomNumber;
    }

    /**
     * randNum setter
     * @param randomNumber value to set
     */
    public void setRandNum(String randomNumber) {
        this.randomNumber = randomNumber;
    }
}
