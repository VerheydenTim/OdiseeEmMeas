/*
********************************************************
2017 Odisee elektromechanica
********************************************************
*/
//Oproepen bibliotheken
#include <WiFi.h>
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>
/*
****************************
Declareren van variabelen
****************************
*/
// Wifi Connection
char ssid[] = "**zelf in te vullen**";      // De naam van het wifi netwerk zoals deze op je PC staat
char pass[] = "**zelf in te vullen**";      // Het paswoord van het wifi netwerk
int keyIndex = 0;                           // Index nummer, enkel bij WEP 
int status = WL_IDLE_STATUS;

//WiFiServer server(80);
char server[] = "www.dweet.io";             // Server waar de meetwaarden naartoe geschreven worden
//Initialiseren van de wifi bibliotheek
WiFiClient client;                          // client is de verwijzing die we gebruiken naar het wifi netwerk

// 
unsigned long lastConnectionTime = 0;             // timer tussen connecties naar server
const unsigned long postingInterval = 60L * 1000L; // delay in ms tussen connecties
String Tijd;

//Ingangen declareren
int DigitalInput = 9;           //Input relatieve vochtigheid
int DigitalInputIR = 10;        //Input personendetectie
int DigitalInputDust = 11;      //Input stofsensor

// Fijn stof sensor
unsigned long duration;         //Duur puls
unsigned long starttime;        //Starttijd puls
unsigned long sampletime_ms = 30000;    //Sample tijd puls
unsigned long lowpulseoccupancy = 0;    //
float ratio = 0;                        //
float concentration = 0;

// Doorsturen gegevens
bool bBool;                       //Dit moet vervangen worden door een case structuur

//relatieve vochtigheid + temperatuur
byte dataFrame [5]; // het in te lezen dataframe afkomstig van de sensor

const int chipSelect = 4;

void setup() {
  
     //Pin configuratie
     pinMode (DigitalInput, OUTPUT); 
     pinMode (DigitalInputIR, INPUT);
     //Open de seriele communicatie met een baudrate van 9600
     Serial.begin(9600);
    
     //Fijn stof meter
      pinMode(11,INPUT);
      starttime = millis();
      /*                                                                                             
     // Wifi connectie
      Serial.println("Poging tot het verkrijgen van een connectie met wifi...");
      status = WiFi.begin(ssid, pass);
    
  // Controleren of het systeem geconnecteerd is
      if ( status != WL_CONNECTED) {
       Serial.println("Connectie met wifi is niet gelukt");
       while(true);
     }
      // De connectie is gelukt
      else {
        Serial.println("Connectie ok");
      }
      // Gegevens van het netwerk printen      
      printWifiStatus();              
*/

      //Initialiseer sd kaart
       Serial.print("Initializing SD card...");
      
        // see if the card is present and can be initialized:
        if (!SD.begin(chipSelect)) {
          Serial.println("Card failed, or not present");
          // don't do anything more:
          return;
        }
        Serial.println("card initialized.");
 }

void loop() {
  start_test ();
  
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
  

  if (digitalRead(DigitalInputIR)==true)
     Serial.println ("Aanwezig");  
    else
     Serial.println ("Afwezig");  
  delay (700); 


  //Fijn stof
  duration = pulseIn(DigitalInputDust, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
Serial.println (millis());  
  if ((millis()-starttime) > sampletime_ms)
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
 
    lowpulseoccupancy = 0;
    starttime = millis();
  }
   Serial.print(lowpulseoccupancy);
    Serial.print(",");
    Serial.print(ratio);
    Serial.print(",");
    Serial.println(concentration);


  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    if (bBool==true)
    {
    httpRequest(dataFrame [0],"POST /dweet/for/ArduinoTestTim?RV=");
    bBool=false;
    }
    else
    {
    httpRequest(dataFrame [2],"POST /dweet/for/ArduinoTestTim?Temp=");
    bBool=true;
    }
  

     // make a string for assembling the data to log:
  String dataString = "";

  // read three sensors and append to the string:
    dataString += "TIJD;";
    dataString += String(millis());
    dataString += ";";
    dataString += "RV";
    dataString += ";";
    dataString += String(dataFrame [0]);
    dataString += ".";
    dataString += String(dataFrame [1]);
    dataString += ";";
    dataString += "Temp";
    dataString += ";";
    dataString += String(dataFrame [2]);
    dataString += ".";
    dataString += String(dataFrame [3]);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
   }

  
/*
********************************************************
Relative vochtigheid en temperatuur sensor communicatie
********************************************************
*/
void start_test () {
  //Eerste puls laag
  digitalWrite (DigitalInput, LOW); // Geef de bus een laag signaal om data overdracht te starten
  delay (30); // Tijd wachten om de sensor de tijd te geven om te reageren
  //Vervolgens puls hoog
  digitalWrite (DigitalInput, HIGH);
  delayMicroseconds (40); // De sensor tijd geven om te reageren
  //Pinmode aanpassen naar input
  pinMode (DigitalInput, INPUT);
  //wachten op puls hoog
  while (digitalRead (DigitalInput) == HIGH);
  delayMicroseconds (80); // Wachten tot de bus een laag signaal geeft
  if (digitalRead (DigitalInput) == LOW);
  delayMicroseconds (80); // Na deze wachttijd klaar om data te verzenden
 
  for (int i = 0; i < 5; i ++) // De 5 bytes afvragen
    dataFrame[i] = read_data ();
 
  pinMode (DigitalInput, OUTPUT);
  digitalWrite (DigitalInput, HIGH); // De bus op true zetten, een laag signaal start opnieuw de transmissie van data
}
 
byte read_data () {
  byte data;
  for (int i = 0; i < 8; i ++) {
    if (digitalRead (DigitalInput) == LOW) {
      while (digitalRead (DigitalInput) == LOW); // wait for 50us
        delayMicroseconds (30); // determine the duration of the high level to determine the data is '0 'or '1'
      if (digitalRead (DigitalInput) == HIGH)
         data |= (1 << (7-i)); // De eerste bit moet helemaal links geplaatst worden dus 7 plaatsen doorschuiven naar links |= is een of bewerking op bit niveau
      while (digitalRead (DigitalInput) == HIGH); // data '1 ', wait for the next one receiver
     }
  }
return data;
}

/*Print wifi status*/
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// COnnectie naar server
void httpRequest(byte dataframe, String post) {
  // Sluit alle huidige connecties
  client.stop();

  if (client.connect(server, 80)) {
      Serial.println("connected");
         // Connectie maken naar server met correcte waarde
      String s = post;
      s.concat(dataframe);
      Serial.println(s);
      client.println(s);

      client.println("Host: www.dweet.io");
      client.println("Connection: close");
      client.println();
      // Tijd bijhouden van laatste connectie
      lastConnectionTime = millis();
    }
    else {
      // Als het connecteren mislukt is
      Serial.println("connection failed");
    }
}
