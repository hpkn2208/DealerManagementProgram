/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package mng;

import data.Account;
import data.AccountChecker;
import data.DealerList;
import java.io.IOException;
import tools.MyTool;

/**
 *
 * @author ADMIN
 */
public class LogIn {
    private Account acc=null;
    
    public LogIn(Account acc){
    this.acc= acc;
}
    public static Account inputAccount(){
        System.out.println("Input username: ");
        String name = MyTool.SC.nextLine();
        System.out.println("Input password: ");
        String password = MyTool.SC.nextLine();
        System.out.println("Input role: ");
        String role = MyTool.SC.nextLine();
        Account acc = new Account(name, password, role);
        return acc;
    }
    
    public Account getAcc(){
        return acc;
    }
    public static void main(String[] args) throws IOException{
        Account acc = null;
        boolean cont = false;
        boolean valid = false;
        do{
            AccountChecker accChk = new AccountChecker();
            acc = inputAccount();
            valid = accChk.check(acc);
            if(!valid)
                cont = MyTool.readBool("invalid account- Try again?");
            if(!valid&& !cont) System.exit(0);
        }
        while (cont);
        LogIn loginObj = new LogIn(acc);
        if (acc.getRole().equalsIgnoreCase("ACC-1")){
            String[] options = {"Add new dealer", "Search a dealer",
                                "Remove a dealer", "Update a dealer", "Print all dealers",
                                "Print continuing dealer", "Print UN-continuing dealer",
                                "Write dealers data to file"};
            Menu mnu = new Menu(options);
            DealerList dList = new DealerList(loginObj);
            dList.initWithFile();
            int choice = 0;
        do{
            choice = mnu.getChoice("Managing dealers");
            switch(choice){
                case 1: dList.addDealer(); break;
                case 2: dList.searchDealer(); break;
                case 3: dList.removeDealer(); break;
                case 4: dList.updateDealer(); break;
                case 5: dList.printAllDealers(); break;
                case 6: dList.printContinuingDealers(); break;
                case 7: dList.printUnContinuingDealers(); break;
                case 8: dList.writeDealerToFile(); break;
                default: 
                    if(dList.isChanged()){
                        boolean res = MyTool.readBool("Data changed. Write to file?");
                        if (res==true) dList.writeDealerToFile();
                    }
            }
        }
        while (choice > 0 && choice <= mnu.size());
        System.out.println("Bye.");
        }
    }
}
