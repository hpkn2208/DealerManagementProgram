/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package data;
import java.io.IOException;
import tools.MyTool;
import java.util.*;
import mng.LogIn;

public class DealerList extends ArrayList<Dealer> {
    LogIn loginObj= null;
    private static final String PHONEPATTERN = "\\d{0}|\\d{11}";
    private String dataFile = "";
    boolean changed = false;

    public DealerList(LogIn loginObj){
        this.loginObj = loginObj;
    }
    private void loadDealerFromFile(){
        List<String> lines = MyTool.readLinesFromFile(dataFile);
        for(String line: lines){
            Dealer d = new Dealer(line);
            this.add(d);
        }
    }
    public void initWithFile(){
        Config cR = new Config();
        dataFile = cR.getDealerFile();
        loadDealerFromFile();
    }
    public DealerList getContinuingList(){
        DealerList list= new DealerList(this.loginObj);
        for(Dealer d: this)
            if(d.isContinuing()==true) list.add(d);
        return list;
    }
    public DealerList getUnContinuingList(){
        DealerList list= new DealerList(this.loginObj);
        for(Dealer d: this)
            if(d.isContinuing()==false) list.add(d);
        return list;
    }
    private int searchDealer(String ID){
        for (int i = 0, n = this.count(); i < n; i++){
            if(this.get(i).getID().equals(ID)) return i;
        }
        return -1;
    }
    public void searchDealer(){
        System.out.println("Enter Dealer ID");
        String ID = MyTool.SC.nextLine();
        int pos = searchDealer(ID);
        if (pos<0) System.out.println("NOT FOUND!");
        else System.out.print(this.get(pos).toString());
    }
    public void addDealer(){
        String ID;
        String name;
        String addr;
        String phone;
        boolean continuing;
        int pos;
        do {
            ID = MyTool.readPattern("ID of new dealer", Dealer.ID_Format);
            ID= ID.toUpperCase();
            pos = searchDealer(ID);
            if (pos>=0)System.out.println("ID is duplicated !");
    }
        while (pos>=0);
            name = MyTool.readNonBlank("Name of new dealer").toUpperCase();
            addr = MyTool.readNonBlank("Address of new dealer");
            phone = MyTool.readPattern("Phone number", Dealer.Phone_Format);
            continuing = true;
            Dealer d = new Dealer(ID, name, addr, phone, continuing);
            this.add(d);
            System.out.println("New dealer has been added.");
            changed= true;
    }
    public int count(){
        return this.size();
    }
    public void removeDealer(){
        System.out.println("Enter Dealer ID");
        String ID = MyTool.SC.nextLine();
        int pos = searchDealer(ID);
        if (pos<0)  System.out.println("Dealer " + ID + " not found !");
        else {
            this.get(pos).setContinuing(false);
            System.out.println("Removed");
            changed = true;
        }
    }
    public void updateDealer(){
        System.out.print("Dealer's ID needs updating: ");
        String ID = MyTool.SC.nextLine();
        int pos = searchDealer(ID);
            if(pos<0) System.out.println("Dealer " + ID + " not found !");
            else {
                Dealer d = this.get(pos);
                String newName ="";
                System.out.print("new name, ENTER for omitting: ");
                newName = MyTool.SC.nextLine().trim().toUpperCase();
                if (!newName.isEmpty()){
                d.setName(newName);
                changed = true;
                }
                String newAddr= "";
                System.out.print("new addresse, ENTER for omitting: ");
                newAddr = MyTool.SC.nextLine().trim();
                if (!newAddr.isEmpty()){
                    d.setAddr(newAddr);
                    changed = true;
                }
                String newPhone="";
                System.out.print("new phone number, ENTER for omitting: ");
                do {
                    newPhone = MyTool.SC.nextLine().trim();
                    boolean valid = MyTool.validStr(newPhone, Dealer.Phone_Format);
                    if (valid){
                        d.setPhone(newPhone);
                        changed = true;
                        newPhone="";
                    }else{
                        System.out.print("Wrong format!! Try again! ");
                    }
                }while(!newPhone.isEmpty());
                System.out.println("Dealers Updated!!");
        }
       }
    public void printAllDealers(){
        if (this.isEmpty()) System.out.println("Empty List!");
            else System.out.println(this);
    }
    public void printContinuingDealers(){
        this.getContinuingList().printAllDealers();
    }
    public void printUnContinuingDealers(){
        this.getUnContinuingList().printAllDealers();
    }
    public void writeDealerToFile() throws IOException{
        if (changed){
            MyTool.writeFile(dataFile, this);
            changed= false;
        System.out.println("Wrote!!");
    }
    }
    public boolean isChanged(){
        return changed;
    }
    public void setChanged(boolean changed){
        this.changed=changed;
    }

}
