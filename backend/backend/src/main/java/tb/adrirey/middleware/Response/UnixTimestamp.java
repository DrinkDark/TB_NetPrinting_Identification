package tb.adrirey.middleware.Response;

import java.util.Vector;

public class UnixTimestamp {
    private long unixTimestamp;

    public UnixTimestamp(long unixTimestamp) {
        this.unixTimestamp = unixTimestamp;
    }

    public long getUnixTimestamp() {
        return unixTimestamp;
    }

    public void setUnixTimestamp(long unixTimestamp) {
        this.unixTimestamp = unixTimestamp;
    }
}
