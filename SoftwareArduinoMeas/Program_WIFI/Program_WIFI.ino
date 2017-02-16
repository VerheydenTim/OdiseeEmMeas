/*
********************************************************
2017 Odisee elektromechanica
********************************************************
*/
//Oproepen bibliotheken
#include <WiFi.h>
#include <Ethernet.h>

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
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers
// Analoge ingang declareren
int DigitalInput = 9;//Input relatieve vochtigheid
int DigitalInputIR = 10;//Input personendetectie
int DigitalInputDust = 11;//Input stofsensor
bool DigitalValue;
int AnalogValue;

// Fijn stof sensor
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

// Doorsturen gegevens
bool bBool;

            /*
            ********************************************************
            Relative vochtigheid en temperatuur sensor communicatie
            ********************************************************
            */
            byte dataFrame [5]; // het in te lezen dataframe afkomstig van de sensor
            //
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

void setup() {
 //Pin configuratie
 pinMode (DigitalInput, OUTPUT);
 pinMode (DigitalInputIR, INPUT);
 //Open de seriele communicatie met een baudrate van 9600
 Serial.begin(9600);


 //Fijn stof meter
   pinMode(11,INPUT);
  starttime = millis();
                                                                                           
                             // Wifi connection
                             // attempt to connect using WPA2 encryption:
                              Serial.println("Attempting to connect to WPA network...");
                              status = WiFi.begin(ssid, pass);
                            
                              // if you're not connected, stop here:
                              if ( status != WL_CONNECTED) {
                               Serial.println("Couldn't get a wifi connection");
                               while(true);
                             }
                             
                              // if you are connected, print out info about the connection:
                              else {
                                Serial.println("Connected to network");
                              }
                              // print your WiFi shield's IP address:
                                Serial.print("IP Address: ");
                                Serial.println(WiFi.localIP());
                                
                              printWifiStatus();
                  
                     
              
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
   }
  
                   

 
}
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

// this method makes a HTTP connection to the server:
void httpRequest(byte dataframe, String post) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  if (client.connect(server, 80)) {
                      Serial.println("connected");
                   
          
                      // Make a HTTP request:
                      String s = post;
                      s.concat(dataframe);
                      Serial.println(s);
                      client.println(s);
                      /*              
                           // Make a HTTP request:
                      s = "POST /dweet/for/ArduinoTestTim?Temp=";
                      s.concat(dataFrame [2]);
                      Serial.println(s);
                      client.println(s);
                      */
                      client.println("Host: www.dweet.io");
                      client.println("Connection: close");
                      client.println();
                      // note the time that the connection was made:
                      lastConnectionTime = millis();
                    }
                    else {
                      // if you couldn't make a connection:
                      Serial.println("connection failed");
                    }
}
