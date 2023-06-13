package tb.adrirey.backend;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.net.SocketException;


@SpringBootApplication
public class BackendApplication {

	public static void main(String[] args) throws SocketException {
		RESTController c = new RESTController();
		SpringApplication.run(BackendApplication.class, args);
	}


}
