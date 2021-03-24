
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

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
  Blynk.notify("There is a gas leakege in restaurant");
  Blynk.email("komaldeora024@gmail.com" , "Alert from Sensor " ,"There is gas leakage in restaurant");
  Blynk.run();
  delay(30000);    // delay of 30s
}
