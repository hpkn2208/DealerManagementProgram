/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package mng;
import tools.MyTool;
import java.util.*;
/**
 *
 * @author ADMIN
 */
public class Menu extends ArrayList<String> {
    public Menu(){
    super();
    }
    public Menu(String[] items){
    super();
    for (String item: items) this.add(item);
}
    public int getChoice(String tittle){
        System.out.println(tittle);
        for(int i=0;i<this.size();i++)
            System.out.println("\t"+(i+1)+"-"+this.get(i));
        return Integer.parseInt(MyTool.SC.nextLine());
    } 
}
