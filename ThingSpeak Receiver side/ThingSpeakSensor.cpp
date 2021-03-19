#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include "SocketClient.h"

using namespace std;
using namespace internetOfThings;


void thingSpeak(int sendData1) // thingSpeak function 
{
   ostringstream head, data;
   cout << "Starting ThingSpeak Example" << endl;
   SocketClient sc("api.thingspeak.com",80);
   data << "field3=" << sendData1 << endl;  // putting data in Field 3 of ThingSpeak
   cout << "Check : "<<sc.connectToServer(); // connecting to thingSpeak server
   head << "POST /update HTTP/1.1\n"
        << "Host: api.thingspeak.com\n"
        << "Connection: close\n"
        << "X-THINGSPEAKAPIKEY: YHOLRJA0W7XREL23\n"  // write API key for thingSpeak channel
        << "Content-Type: application/x-www-form-urlencoded\n"
        << "Content-Length:" << string(data.str()).length() << "\n\n";  // calculating string Length
   sc.send(string(head.str()));
   sc.send(string(data.str())); // submittng data in string
   string rec = sc.receive(1024);
   cout << "[" << rec << "]" << endl;
   cout << "End of ThingSpeak Example" << endl;  
}

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
   thingSpeak(getTemp()); //putting value from getTemp function to thingSpeak function
   sysytm("sleep 15");   // putting system to sleep for 15 sec  
   }  
 
   return 0;
}
