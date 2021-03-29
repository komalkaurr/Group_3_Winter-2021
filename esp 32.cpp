#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D1,D2); // RX, TX
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
  mySerial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  recieve=mySerial.readString();
  Serial.println(recieve);
  if(recieve=="alert")
  {
    Blynk.notify("Attention ! Gas Leakage.");
//  Blynk.email("2262010929@fido.ca" , "Alert from Sensor " ,"There is gas leakage in restaurant");
  }
    Blynk.run();

  delay(2000);    // delay of 60s
}

