import java.util.Scanner;
import java.io.*;

class main
{
    public static void main(String[] args)
    {
        /*try
        {
            //cd C:\Program Files\PaperCut MF\server\bin\win\
            Runtime.getRuntime().exec("cmd /c start cmd.exe /K");
        }
        catch (Exception e)
        {
            System.out.println("Error ");
            e.printStackTrace();
        }*/

        ProcessBuilder builder = new ProcessBuilder( "cmd.exe" );
        Process p=null;
        try {
            p = builder.start();
        }
        catch (IOException e) {
            System.out.println(e);
        }

        BufferedWriter p_stdin = new BufferedWriter(new OutputStreamWriter(p.getOutputStream()));

        try {
            p_stdin.write("psexec \\\\PaperCutServer cmd");
            p_stdin.newLine();
            p_stdin.flush();
            p_stdin.write("cd C:\\Program Files\\PaperCut MF\\server\\bin\\win");
            p_stdin.newLine();
            p_stdin.flush();
            p_stdin.write("server-command get-user-property demo balance");
            p_stdin.newLine();
            p_stdin.flush();
        }
        catch (IOException e) {
            System.out.println(e);
        }

        try {
            p_stdin.write("exit");
            p_stdin.newLine();
            p_stdin.flush();
        }
        catch (IOException e) {
            System.out.println(e);
        }

        Scanner s = new Scanner(p.getInputStream());
        while (s.hasNext())
        {
            System.out.println( s.next() );
        }
        s.close();
    }
}

