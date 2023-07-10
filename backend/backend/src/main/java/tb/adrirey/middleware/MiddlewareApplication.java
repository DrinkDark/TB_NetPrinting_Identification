package tb.adrirey.middleware;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.net.SocketException;


@SpringBootApplication
public class MiddlewareApplication {

	public static void main(String[] args) throws SocketException {
		RESTController restC = new RESTController();
		SpringApplication.run(MiddlewareApplication.class, args);
	}


}
