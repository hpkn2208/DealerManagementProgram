/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package data;

/**
 *
 * @author ADMIN
 */
public class Account {
    private String accName;
    private String pwd;
    private String role; 
    
public Account(String accName, String pwd, String role){
    this.accName = accName;
    this.pwd = pwd;
    this.role = role;
}
public String getAccName(){
    return accName;
}
public String getPwd(){
    return pwd;
}
public String getRole(){
    return role;
}

}
