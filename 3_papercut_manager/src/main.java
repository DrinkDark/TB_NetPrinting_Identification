import java.util.Scanner;
import java.io.*;


class main {
    public static void main(String[] args) throws IOException {
        ServerCommandProxy scp = new ServerCommandProxy("PaperCutServer", 9191, "authToken");
        GUI gui = new GUI(scp);
    }
}

