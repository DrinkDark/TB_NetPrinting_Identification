package tb.adrirey.middleware.Response;

/**
 * Random number class
 */
public class RandNum {
    private String randNum;

    /**
     * Default constructor
     *
     * @param randNum value to set
     */
    public RandNum(String randNum) {
        this.randNum = randNum;
    }

    /**
     * randNum getter
     *
     * @return randNum
     */
    public String getRandNum() {
        return randNum;
    }

    /**
     * randNum setter
     * @param randNum value to set
     */
    public void setRandNum(String randNum) {
        this.randNum = randNum;
    }
}
