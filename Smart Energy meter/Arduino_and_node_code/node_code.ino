#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WLAN_SSID       "redmi"
#define WLAN_PASS       "mnbvcxzl"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "gokulakrishnan"
#define AIO_KEY         "112618e7fe8b47f5a61405d7c5ded1b4"

WiFiClient client;
  //pin declaration
  const int relay1 = 16;
  const int relay2 = 5;
  const int relay3 = 4;

  //variable declaration
  int billtopay ;   
  unsigned int energyTariff = 5; 

  //String declaration to receive data
  char watt[5];

  //eeprom address
  //int a = 100;
  int a;
  int val;
  double temp = 0.0f;
  int demandpub;
  int masterpub;
  unsigned long m = millis();
  int j = 0;

//adafruit link
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Power = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/unit"); //unit consumption sending
Adafruit_MQTT_Publish realtimebill = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bill");//real time
Adafruit_MQTT_Publish billamountpre = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/prepaid"); //prepaid
Adafruit_MQTT_Publish billamountpost = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/postpaid");//postpaid
Adafruit_MQTT_Subscribe demandcontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/f/demand"); //demand
Adafruit_MQTT_Subscribe mastercontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/f/master"); //master
Adafruit_MQTT_Subscribe modecontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/f/mode"); //mode
Adafruit_MQTT_Subscribe postcontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/f/date"); //mode postpaid


void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  
  mqtt.subscribe(&demandcontrol);
  mqtt.subscribe(&mastercontrol);
  mqtt.subscribe(&modecontrol);
  mqtt.subscribe(&postcontrol);
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  
  
 //digitalWrite(relay1, LOW);
 //digitalWrite(relay2, LOW);

 // EEPROM.write(a, 0);
 m = 0;
}

void loop() 
{

  MQTT_connect();
    int i=0;
    float watt1;
    if(Serial.available() > 0 )
    {
      delay(100); //allows all serial sent to be received together
      while(Serial.available() && i<5) 
      {
        watt[i++] = Serial.read();
      }
    }
      watt[i++]='\0';
      watt1 = atof(watt);
      //Serial.println(m);
      if(((m/1000) % 50) == 0)
      {
        if(masterpub == 0)
        {
          j = 0;
          temp = temp + 0.0f;
        }
        if(demandpub == 0)
        { 
          j++;
          if(j == 1)
          {
            temp = temp + 0.13f;
          }
          temp = temp + 0.05f;
        }
        if( (masterpub == 1)&&(demandpub == 1))
        {
          j = 0;
          temp = temp + 0.15f;
        }
        //publish status
        if (! Power.publish(temp)) 
        {
          Serial.println(F("Failed"));
        } 
        else 
        {
          Serial.println(F("OK! unit"));
        }
        billtopay = temp*energyTariff; 
        // real time bill sending
        if (! realtimebill.publish(billtopay)) 
        { 
          Serial.println(F("Failed"));
        }
        else 
        {
          Serial.println(F("OK! bill"));
        }
        //mode switching 
        //prepaid bill
        if( a == 0)
        {
          if (! billamountpre.publish(billtopay)) 
          {
            Serial.println(F("Failed"));
          }
          else 
          {
            Serial.println(F("OK!"));
          }
        }
        //Postpaid bill
        if( a == 1 )
        {
          if( val == 1)
          {
            if (! billamountpost.publish(billtopay)) 
            {
              Serial.println(F("Failed"));
            }
            else 
            {
              Serial.println(F("OK!"));
            }      
          }
        }
      }
      Serial.println(temp);
     

  //mode switching 
  // ada read data
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //demand control
    if (subscription == &demandcontrol)
    {
      Serial.print("onoff demandcontrol: ");
      Serial.println((char*) demandcontrol.lastread);
      
      if (!strcmp((char*) demandcontrol.lastread, "ON"))
      {
        digitalWrite(relay1, LOW);
        demandpub = 0;
        Serial.println(demandpub);
      }
      else
      {
        digitalWrite(relay1, HIGH);
        demandpub = 1;
        Serial.println(demandpub);
      }
    }
    
    //Master control
    if (subscription == &mastercontrol)
    {
      Serial.print("onoff Master: ");
      Serial.println((char*) mastercontrol.lastread);
     
      if (!strcmp((char*) mastercontrol.lastread, "ON"))
      {
        digitalWrite(relay2, LOW);
        masterpub = 0;
        Serial.println(masterpub);
      }
      else
      {
        digitalWrite(relay2, HIGH);
        masterpub = 1;
        Serial.println(masterpub);
      }
    }
    
    //mode control
    if (subscription == &modecontrol)
    {
      Serial.print("onoff modecontrol: ");
      Serial.println((char*) modecontrol.lastread);
      
      if (!strcmp((char*) modecontrol.lastread, "Postpaid"))
      {
        // EEPROM.update(a, 0); //prepaid
         a = 0;
         Serial.println(a);
      }
      else
      {
          //EEPROM.update(a, 1); //postpaid
          a = 1;
          Serial.println(a);
      }
    }

    
    // postpaid control
    if (subscription == &postcontrol)
    {
      Serial.print("onoff postcontrol: ");
      Serial.println((char*) postcontrol.lastread);
      
      if (!strcmp((char*) postcontrol.lastread, "1"))
      {
        val = 0; 
        Serial.println(val);
      }
      else
      {
        val = 1;
        Serial.println(val);
      }
    }
  }
delay(5000);

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
