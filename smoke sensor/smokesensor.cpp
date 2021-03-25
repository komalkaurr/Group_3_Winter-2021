#include <iostream> //provides basic input and output services for C++ programs.
#include <unistd.h> //provides access to the POSIX operating system API.
#include <fstream>  //performs input/output operations on the file
#include <math.h>  // for mathematical operation

using namespace std;


int sensor_val()  // generating a temerture function
{
   int Smoke_sensor;  // generating a local variable
   fstream f; // generating a stream for file
   f.open("/sys/bus/iio/devices/iio:device0/in_voltage1_raw"); //  config P9_40 AIN of BBBW 
   f>>Smoke_sensor; // giving file address to local variable
   return (int)Smoke_sensor; //returning integer value to getTemp function
}
 

int main() 
{
  float dex,sensor_output,b ;
   while(1)
   {
       dex= sensor_val();
      b=dex;
      b=(b/4);
   if ((int)b>=300)
   cout<< "smoke detected "<< endl;
   dex=(dex/4096)*5;
   sensor_output=3.027*exp(1.0698*dex); 
   cout<<"Smoke Sensor Data :" << sensor_output <<"ppm"<< endl;   
   system("sleep 1");   // putting system to sleep for 15 sec  
   }  
 
   return 0;
}
