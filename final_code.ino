#include<stdlib.h>
#include <SoftwareSerial.h>
#include "DHT.h"

#define SSID "ACL main"//your network name
#define PASS "uiouiouio"//your network password
#define IP "184.106.153.149" // thingspeak.com
#define Baud_Rate 115200 //Another common value is 9600
#define DELAY_TIME 10000 //time in ms between posting data to ThingSpeak
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial mySerial(10, 11);

String GET = "GET /update?key=IXON38AEB5HWAW59&field1=";
String FIELD2 = "&field2=";

//if you want to add more fields this is how
//String FIELD3 = "&field3=";

bool updated;
bool connected;

//this runs once
void setup()
{
  mySerial.begin(Baud_Rate);
  dht.begin();
  mySerial.println("AT");
  delay(2000);
  if (mySerial.find("OK")) {
    delay(3000);
    //connect to your wifi netowork
    connected = connectWiFi();
  }
}

//this runs over and over
void loop() {
 
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  mySerial.println(h);
  mySerial.println(t);



  //update ThingSpeak channel with new values
  updated = updateTemp(String(t), String(h));


  //wait for delay time before attempting to post again
  delay(DELAY_TIME);
}

bool updateTemp(String tenmpF, String humid) {
  //initialize your AT command string
  String cmd = "AT+CIPSTART=\"TCP\",\"";

  //add IP address and port
  cmd += IP;
  cmd += "\",80";

  //connect
  mySerial.println(cmd);
  delay(2000);
  if (mySerial.find("Error")) {
    return false;
  }

  //build GET command, ThingSpeak takes Post or Get commands for updates, I use a Get
  cmd = GET;
  cmd += tenmpF;
  cmd += FIELD2;
  cmd += humid;

  //continue to add data here if you have more fields such as a light sensor
  //cmd += FIELD3;
  //cmd += <field 3 value>

  cmd += "\r\n";

  //Use AT commands to send data
  mySerial.print("AT+CIPSEND=");
  mySerial.println(cmd.length());
  if (mySerial.find(">")) {
    //send through command to update values
    mySerial.print(cmd);
  } else {
    mySerial.println("AT+CIPCLOSE");
  }

  if (mySerial.find("OK")) {
    //success! Your most recent values should be online.
    return true;
  } else {
    return false;
  }
}

boolean connectWiFi() {
  //set ESP8266 mode with AT commands
  mySerial.println("AT+CWMODE=1");
  delay(2000);
  String cmd ="AT+CWJAP=";
  cmd +='"';
  cmd +=SSID;
  cmd +='"';
  cmd +=',';
  cmd +='"';
  cmd +=PASS;
  cmd +='"';

  //connect to WiFi network and wait 5 seconds
  mySerial.println(cmd);
  delay(5000);

  //if connected return true, else false
  if (mySerial.find("OK")) {
    return true;
  } else {
    return false;
  }
}

