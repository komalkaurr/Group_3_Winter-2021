#include<iostream>            //This header file contains all the functions of program like cout, cin etc.
#include<cstring>             //This header file declares a set of functions to work with C style string.
#include<unistd.h>            // This header file is for miscellaneous functions declaration.
#include "buzzer.h"           //It contains buzzer function.

#define LOW 0
#define HIGH 1

using namespace std;

int pin_number=67;            //This is a GPIO_67 of Beaglebone Black Wireless
int getTemp()  // generating a temerture function
{
   float calculated_temp;  // generating a local variable
   fstream f; // generating a stream for file
   f.open("/sys/bus/iio/devices/iio:device0/in_voltage1_raw"); //  config P9_40 AIN of BBBW 
   f>>calculated_temp; // giving file address to local variable
   calculated_temp= (calculated_temp/22.75);  // formula for calculating temp in celsius
   return (int)calculated_temp; //returning integer value to getTemp function
}

int main()
{
while(1)
{
    if (getTemp()>= 15 )
    {
    pinSetup(pin_number,"out");      //Setting GPIO_67 as output.
    output(pin_number,HIGH);         //Sending high (1) logic to GPIO_67.
        cout<<"fan is ON"<<endl;
        system("sleep 1");
        }  
    else {            //delay
    pinSetup(pin_number,"out");       //Setting GPIO_67 as output.
    output(pin_number,LOW);           //Sending Low(0) logic to GPIO_67
        cout<<"fan is OFF"<<endl;
        system("sleep 1");
        }                //delay
}
    return 0;
}