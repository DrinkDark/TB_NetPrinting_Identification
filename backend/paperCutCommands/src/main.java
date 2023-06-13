import java.util.Scanner;
import java.io.*;

class main {
    public static void main(String[] args) throws IOException {
        try {
            ProcessBuilder processBuilder = new ProcessBuilder("cmd.exe", "/c", "psexec \\\\PaperCutServer cmd && cd C:\\Program Files\\PaperCut MF\\server\\bin\\win && serve-command get-user-property demo balance");

            Process process = processBuilder.start();

            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line;
            while ((line = reader.readLine()) != null) {
                System.out.println(line);
            }

            process.waitFor();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}

