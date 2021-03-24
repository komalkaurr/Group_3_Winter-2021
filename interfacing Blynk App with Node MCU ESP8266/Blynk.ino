
#define BLYNK_PRINT Serial   //The compiler will replace any mention of BLYNK_PRINT with the Serial at compile time. 



#include <ESP8266WiFi.h>       //This library provides ESP8266 specific Wi-Fi routines that we are calling to connect to the network.

#include <BlynkSimpleEsp8266.h>           //This library is associated with  Blynk application


String recieve; //global string declaration for recieveing data

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon). 
char auth[] = "sEBpNn9m6oZd5h5-zHmFwhApmzI_c1fj";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "kalras";
char pass[] = "Moffatt27";

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.notify("There is a gas leakege in restaurant");           //This function is associated with notification widget. We can print any string by passing it in its argument.

  Blynk.email("komaldeora024@gmail.com" , "Alert from Sensor " ,"There is gas leakage in restaurant");   //Thisfunction is associated with email notification.

  Blynk.run();                   //Blynk.run() is a main Blynk routine responsible for keeping connection alive, sending data, receiving data, etc.

  delay(30000);    // delay of 30s
}
