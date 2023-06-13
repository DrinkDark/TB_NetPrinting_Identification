
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Vector;

public class GUI extends JFrame{

        public GUI(ServerCommandProxy scp) {
            super("PaperCut manager");
            setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            setLayout(new FlowLayout(FlowLayout.CENTER));

            //Create buttons
            JButton setBalanceButton = new JButton("Set balance");
            JButton addUserButton = new JButton("Add user");
            JButton addUsersFromFileButton = new JButton("Add users from file");
            JButton deleteUserButton = new JButton("Delete user");
            JButton deleteAllUsersButton = new JButton("Delete all users");

            //Set buttons size
            Dimension buttonSize = new Dimension(250, 50);
            setBalanceButton.setPreferredSize(buttonSize);
            addUserButton.setPreferredSize(buttonSize);
            addUsersFromFileButton.setPreferredSize(buttonSize);
            deleteUserButton.setPreferredSize(buttonSize);
            deleteAllUsersButton.setPreferredSize(buttonSize);

            //Set balance button listener
            setBalanceButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    String name = JOptionPane.showInputDialog(null, "Enter name:");
                    String balance = JOptionPane.showInputDialog(null, "Enter balance:");

                    if(!name.isEmpty() && name != null && scp.isUserExists(name)){
                        if(!balance.isEmpty()){
                            scp.setUserAccountBalance(name, Double.parseDouble(balance), "");
                        }else {
                            scp.setUserAccountBalance(name, 0.0, "");
                        }
                    } else {
                        JOptionPane.showMessageDialog(null, "Error! Enter a valid name.", "Error", JOptionPane.ERROR_MESSAGE);
                    }
                }
            });

            //Add user button listener
            addUserButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    String name = JOptionPane.showInputDialog(null, "Enter name : ");
                    String primaryCardNumber = JOptionPane.showInputDialog(null, "Enter primary card number : ");
                    String secondaryCardNumber = JOptionPane.showInputDialog(null, "Enter secondary card number : ");
                    String balance = JOptionPane.showInputDialog(null, "Enter balance : ");

                    if(!name.isEmpty() && name != null && !scp.isUserExists(name)){
                        scp.addNewUser(name);
                        scp.setUserProperty(name, "primary-card-number", primaryCardNumber);
                        scp.setUserProperty(name, "secondary-card-number", secondaryCardNumber);
                        if(!balance.isEmpty()){
                            scp.setUserProperty(name, "balance", balance);
                        }else {
                            scp.setUserProperty(name, "balance", "0");
                        }
                    } else {
                        JOptionPane.showMessageDialog(null, "Error! Enter a valid name.", "Error", JOptionPane.ERROR_MESSAGE);
                    }

                }
            });

            //Add users from list button listener
            addUsersFromFileButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    String filePath = JOptionPane.showInputDialog(null, "Enter file path:");

                    try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
                        String line;
                        while ((line = br.readLine()) != null) {
                            String data[] = line.split(",");
                            String name = data[0];
                            String primaryCardNumber = data[1];
                            String secondaryCardNumber = data[2];
                            String balance = data[3];

                            if(!scp.isUserExists(name)) {
                                scp.addNewUser(name);
                                scp.setUserProperty(name, "primary-card-number", primaryCardNumber);
                                scp.setUserProperty(name, "secondary-card-number", secondaryCardNumber);
                                scp.setUserProperty(name, "balance", balance);
                            }
                        }
                    } catch (IOException ex) {
                        JOptionPane.showMessageDialog(null, "Error! Enter a valid path.", "Error", JOptionPane.ERROR_MESSAGE);
                    }
                }
            });

            //Delete user button listener
            deleteUserButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    String name = JOptionPane.showInputDialog(null, "Enter name:");
                    if(!name.isEmpty() && name != null && scp.isUserExists(name)){
                        int confirmation = JOptionPane.showConfirmDialog(null, "Are you sure you want to delete " + name + "?");

                        if (confirmation == JOptionPane.YES_OPTION) {
                            scp.deleteExistingUser(name);
                        }
                    } else {
                        JOptionPane.showMessageDialog(null, "Error! Enter a valid name.", "Error", JOptionPane.ERROR_MESSAGE);
                    }
                }
            });

            //delete all users button listener
            deleteAllUsersButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    int confirmation = JOptionPane.showConfirmDialog(null, "Are you sure you want to delete all users?");

                    if (confirmation == JOptionPane.YES_OPTION) {
                        Vector<String> users = scp.listUserAccounts(0, 1000);
                        for (int i = 0; i < users.size(); i++){
                            scp.deleteExistingUser(users.get(i));
                        }
                    }
                }
            });

            //Add buttons in the frame
            add(setBalanceButton);
            add(addUserButton);
            add(addUsersFromFileButton);
            add(deleteUserButton);
            add(deleteAllUsersButton);

            setSize(300, 320);
            setLocationRelativeTo(null);
            setVisible(true);
        }
}
