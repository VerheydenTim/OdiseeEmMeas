/*
********************************************************
2017 Odisee elektromechanica
********************************************************
*/
//Bibliotheken ethernet
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
#include <Twitter.h>
#include <util.h>

/*Voor de DHT11 sensor wordt een bibliotheek gebruikt
In deze bibliotheek zit de omzetting van het signaal van 1 pin */
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN            9         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 

//SD kaart
#include <SD.h>

/*
****************************
Declareren van variabelen
****************************
*/
//WiFiServer server(80);
char server[] = "www.dweet.io";             // Server waar de meetwaarden naartoe geschreven worden
//Initialiseren van de wifi bibliotheek
EthernetClient client;                          // client is de verwijzing die we gebruiken naar het wifi netwerk

// 
unsigned long lastConnectionTime = 0;             // timer tussen connecties naar server
unsigned long lastConnectionTime2 = 0;             // timer tussen connecties naar server
const unsigned long postingInterval = 10L * 1000L; // delay in ms tussen connecties
String Tijd;


int DigitalInput = 9;
// Fijn stof sensor
bool bActiveFlash =false;
bool bActiveEthernet =false;

// Doorsturen gegevens
bool bBool;                       //Dit moet vervangen worden door een case structuur

DHT_Unified dht(DigitalInput, DHTTYPE);
//relatieve vochtigheid + temperatuur
byte dataFrame [5]; // het in te lezen dataframe afkomstig van de sensor

const int chipSelect = 4;

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xC5, 0xC2 };
void setup() {
   dht.begin();
     sensor_t sensor;


     //Open de seriele communicatie met een baudrate van 9600
     Serial.begin(9600);
    



  //Initialiseer sd kaart
   Serial.print("Initializing SD card...");
  
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
    }
   // Serial.println("card initialized.");


  //Connectie maken met het netwerk
  Ethernet.begin(mac); 

  delay(5000);
  //Serial.println("connecting...");
  lastConnectionTime=millis();
 }

void loop() {
  if (((millis() - lastConnectionTime) > postingInterval) and bActiveEthernet==false) {
      bActiveFlash=true;
  
      // Get temperature event and print its value.
      sensors_event_t event;  
      dht.temperature().getEvent(&event);
      if (isnan(event.temperature)) {
        Serial.println("Error reading temperature!");
      }
      else {
        dataFrame [2]=event.temperature;
      }
      // Get humidity event and print its value.
      dht.humidity().getEvent(&event);
      if (isnan(event.relative_humidity)) {
        Serial.println("Error reading humidity!");
      }
      else {
        dataFrame [0]=event.relative_humidity;
      }
    /*
      Serial.print ("Current humdity =");
      Serial.print (dataFrame [0], DEC); //Gehele getallen;
      Serial.print ('.');
      Serial.print (dataFrame [1], DEC); //Decimale getallen;
      Serial.println ('%');
      Serial.print ("Current temperature =");
      Serial.print (dataFrame [2], DEC); // Gehele getallen;
      Serial.print ('.');
      Serial.print (dataFrame [3], DEC); //Decimale getallen;
      Serial.println ('C');
       */
       // make a string for assembling the data to log:
      String dataString = "";

      // read three sensors and append to the string:
        dataString += "TIJD;";
        dataString += String(millis());
        dataString += ";";
        dataString += "RV";
        dataString += ";";
        dataString += String(dataFrame [0]);
        dataString += ".0";
        dataString += ";";
        dataString += "Temp";
        dataString += ";";
        dataString += String(dataFrame [2]);
        dataString += ".0";

      // so you have to close this one before opening another.
      File dataFile = SD.open("datalog.csv", FILE_WRITE);
    
      // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
         lastConnectionTime = millis();
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening datalog.csv");
      }
  }
  else
    bActiveFlash=false;
  

if ((millis() - lastConnectionTime2) > (postingInterval+2000) and  bActiveFlash==false)
{
    bActiveEthernet=true;
   if (bBool==true)
    {
      client.stop();
     String s ="POST /dweet/for/ArduinoTest?RV=";
      if (client.connect(server, 80)) {
      s.concat(dataFrame [0]);
      Serial.println(s);
      client.println(s);

      client.println("Host: www.dweet.io"); 
      client.println("Connection: close");
      client.println();
      }
   
    bBool=false;
    lastConnectionTime2=millis();
    lastConnectionTime=millis();
     }
    
    else
    {
       client.stop();
     String s ="POST /dweet/for/ArduinoTestTim?Temp=";
      if (client.connect(server, 80)) {
      s.concat(dataFrame [2]);
      Serial.println(s);
      client.println(s);

      client.println("Host: www.dweet.io"); 
      client.println("Connection: close");
      client.println();
      }
    bBool=true;
    lastConnectionTime2=millis();
     lastConnectionTime=millis();
    }
}
   
   else  
   bActiveEthernet=false;
}


