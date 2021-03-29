#include <iostream> //provides basic input and output services for C++
#include <unistd.h> //provides access to the POSIX operating system API. #include <fstream> //performs input/output operations on the file 
#include <math.h> // for mathematical operation 
#include <stdio.h> //defines various functions for input output operations 
#include <fcntl.h> //Used to define arguments for open ,close functions #include <unistd.h> //This is for misscelaneous functions declaration 
#include <termios.h> // It contains the definitions used by terminal input output interfaces 
#include <string.h> 
#include <cstring> 


using namespace std; 

int sensor_val() // generating a temerture function 
{ 
int Smoke_sensor; // generating a local variable 
fstream f; // generating a stream for file f.open("/sys/bus/iio/devices/iio:device0/in_voltage1_raw"); // config P9_40 AIN of BBBW 
f>>Smoke_sensor; // giving file address to local variable 
return (int)Smoke_sensor; //returning integer value to getTemp function 
} 


int main() 
{
float dex,sensor_output,b; 
int file, count, pin_number; 
file = open("/dev/ttyO1", O_RDWR); //this function open the file associated with Uart1
 struct termios options; //declaration of a struct options 
tcgetattr(file, &options); //This function get parameters associated with terminal and store them in termios 
options.c_cflag = B9600 | CS8 | CREAD | CLOCAL; 
options.c_iflag = IGNPAR | ICRNL; 
tcflush(file, TCIFLUSH); //This function is used to clear input and output queue associated with terminal files 
tcsetattr(file, TCSANOW, &options); 



char receive[] = "alert"; //recieve array declaration 
while(1)  {
 dex= sensor_val(); 
 sensor_output = (dex * 5)/ 4095;    sensor_output=3.027*exp(sensor_output*1.0698);
cout<<"CO Sensor Reading : " << (int)sensor_output<<" ppm"<<endl; if((int)sensor_output > 620)
cout<<“Gas Leakage Alert” endl;  {
 write(file, receive, strlen(receive)); 
} 
system("sleep 2"); // putting system to sleep 
} 
return 0;
 }
