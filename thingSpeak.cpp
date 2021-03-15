#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include "SocketClient.h"

using namespace std;
using namespace internetOfThings;


void thingSpeak(int sendData){
   ostringstream head, data;
   cout << "Starting ThingSpeak Example" << endl;
   SocketClient sc("api.thingspeak.com",80);   // socketClient header file access through this function
   data << "field1=" << sendData << endl;  // sending data to thre field 1 on the channel
   cout << "Check : "<<sc.connectToServer();  //setting connection with server
   head << "POST /update HTTP/1.1\n"  // adding the http request from bbb wireless
        << "Host: api.thingspeak.com\n"  // host adress 
        << "Connection: close\n"
        << "X-THINGSPEAKAPIKEY: L41OHUV7IBZ01UWC\n"  //write api key for the channel on thingSpeak
        << "Content-Type: application/x-www-form-urlencoded\n"  // describing the content type 
        << "Content-Length:" << string(data.str()).length() << "\n\n";  // calculating the string length 
   sc.send(string(head.str()));
   sc.send(string(data.str()));  // snding the total amount to thingspeak
   string rec = sc.receive(1024);
   cout << "[" << rec << "]" << endl;
   cout << "End of ThingSpeak Example" << endl;
}


int main() {
   int calculated_amount=850;  // adding the amount to a varible
   thingSpeak(calculated_amount);  // calling the thingSpeak function
   return 0;
}
