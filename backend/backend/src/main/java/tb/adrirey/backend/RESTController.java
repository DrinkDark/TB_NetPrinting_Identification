package tb.adrirey.backend;

import org.springframework.web.bind.annotation.*;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

@RestController
public class RESTController {
    ServerCommandProxy scp;
    User user;

    public RESTController() {
        user = new User();
        scp = new ServerCommandProxy("PaperCutServer", 9191, "authToken");
    }

    @RequestMapping("/userName")
    public String getName(@RequestParam String data) {
        return scp.lookUpUserNameByCardNo(data);
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
