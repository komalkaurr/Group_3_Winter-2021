#include<iostream>          //It contains  all the functions of program like cout, cin etc.
#include<sstream>           //This header provide string stream.
#include<fstream>          //Inputs/Outputs stram class to operate on files.
using namespace std; 
ostringstream out_string;    //used to operate on strings and to operate on variables of different data types
string PATH="/sys/class/gpio/gpio";       //define GPIO path.
fstream fd;

void pinSetup(int pinNumber, string direction){
    out_string << pinNumber;
    string newS=PATH+out_string.str();
    out_string.str("");
    out_string.clear();
    fd.open(newS+"/direction");
    fd << direction;
    fd.close();

}

 void output(int pinNumber, bool logic){
    out_string << pinNumber;
    string newS=PATH+out_string.str();
    out_string.str("");
    out_string.clear();
    out_string << int(logic);
    fd.open(newS+"/value");
    fd << int(logic);
    out_string.str("");
    out_string.clear();
    fd.close();
 }