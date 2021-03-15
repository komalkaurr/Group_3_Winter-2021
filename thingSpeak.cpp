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
   SocketClient sc("api.thingspeak.com",80);
   data << "field1=" << sendData << endl;
   cout << "Check : "<<sc.connectToServer();
   head << "POST /update HTTP/1.1\n"
        << "Host: api.thingspeak.com\n"
        << "Connection: close\n"
        << "X-THINGSPEAKAPIKEY: L41OHUV7IBZ01UWC\n"
        << "Content-Type: application/x-www-form-urlencoded\n"
        << "Content-Length:" << string(data.str()).length() << "\n\n";
   sc.send(string(head.str()));
   sc.send(string(data.str()));
   string rec = sc.receive(1024);
   cout << "[" << rec << "]" << endl;
   cout << "End of ThingSpeak Example" << endl;
}


int main() {
   int calculated_amount=850;
   thingSpeak(calculated_amount);
   return 0;
}
