package tb.adrirey.backend;

import org.springframework.web.bind.annotation.*;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

@RestController
public class Controller {
    User user = new User();

    @RequestMapping("/userName")
    public String getName() {
        return user.getName() + " " + user.getSurname();
    }

    @RequestMapping("/credit")
    public float getCredit() {
        return user.getCredit();
    }

    @RequestMapping("/userID")
    public int getUserID() {
        return user.getUserID();
    }

    @RequestMapping("/encryptData")
    public String aesEncryptedData(@RequestParam String data) throws NoSuchPaddingException, IllegalBlockSizeException, NoSuchAlgorithmException, BadPaddingException, InvalidKeyException {
        return user.aesEncryptedData(data);
    }
}
