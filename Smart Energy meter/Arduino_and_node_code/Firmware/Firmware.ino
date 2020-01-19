
// Include Libraries
#include "Arduino.h"
#include "ACS712.h"
#include "Buzzer.h"
#include "ESP8266.h"
#include "LiquidCrystal.h"
#include "Wire.h"
#include "RTClib.h"
#include "Relay.h"


// Pin Definitions
#define ACS712_PIN_VO	A1
#define BUZZER_PIN_SIG	4
#define LCD_PIN_RS	11
#define LCD_PIN_E	10
#define LCD_PIN_DB4	5
#define LCD_PIN_DB5	6
#define LCD_PIN_DB6	7
#define LCD_PIN_DB7	8
#define RELAYMODULE1_1_PIN_SIGNAL	12
#define RELAYMODULE2_2_PIN_SIGNAL	13



// Global variables and defines
const int acs712calFactor = 513;
const char *SSID     = "WIFI-SSID"; // Enter your Wi-Fi name 
const char *PASSWORD = "PASSWORD" ; // Enter your Wi-Fi password

char* const host = "www.google.com";
int hostPort = 80;
// object initialization
ACS712 acs712(ACS712_PIN_VO);
Buzzer buzzer(BUZZER_PIN_SIG);
ESP8266 wifi;
LiquidCrystal lcd(LCD_PIN_RS,LCD_PIN_E,LCD_PIN_DB4,LCD_PIN_DB5,LCD_PIN_DB6,LCD_PIN_DB7);
RTC_DS3231 rtcDS;
Relay relayModule1_1(RELAYMODULE1_1_PIN_SIGNAL);
Relay relayModule2_2(RELAYMODULE2_2_PIN_SIGNAL);


// define vars for testing menu
const int timeout = 10000;       //define timeout of 10 sec
char menuOption = 0;
long time0;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() 
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    Serial.println("start");
    
    //Manually calibarte the ACS712 current sensor.
    //Connet the ACS to your board, but do not connect the current sensing side.
    //Follow serial monitor instructions. This needs be done one time only.
    acs712.calibrate(acs712calFactor);
    wifi.init(SSID, PASSWORD);
    // set up the LCD's number of columns and rows
    lcd.begin(16, 2);
    if (! rtcDS.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
    }
    if (rtcDS.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtcDS.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtcDS.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    menuOption = menu();
    
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() 
{
    
    
    if(menuOption == '1') {
    // Current Sensor Module ACS712 - Test Code
    //Get averaged current measurment.
    float acs712Currrent  = acs712.getCurrent();
    Serial.print(acs712Currrent); Serial.println(F(" [mA]"));

    }
    else if(menuOption == '2') {
    // Piezo Buzzer - Test Code
    // The buzzer will turn on and off for 500ms (0.5 sec)
    buzzer.on();       // 1. turns on
    delay(500);             // 2. waits 500 milliseconds (0.5 sec). Change the value in the brackets (500) for a longer or shorter delay in milliseconds.
    buzzer.off();      // 3. turns off.
    delay(500);             // 4. waits 500 milliseconds (0.5 sec). Change the value in the brackets (500) for a longer or shorter delay in milliseconds.
    }
    else if(menuOption == '3') {
    // Logic Level Converter - Bi-Directional - Test Code
    //Send request for www.google.com at port 80
    wifi.httpGet(host, hostPort);
    // get response buffer. Note that it is set to 250 bytes due to the Arduino low memory
    char* wifiBuf = wifi.getBuffer();
    //Comment out to print the buffer to Serial Monitor
    //for(int i=0; i< MAX_BUFFER_SIZE ; i++)
    //  Serial.print(wifiBuf[i]);
    //search buffer for the date and time and print it to the serial monitor. This is GMT time!
    char *wifiDateIdx = strstr (wifiBuf, "Date");
    for (int i = 0; wifiDateIdx[i] != '\n' ; i++)
    Serial.print(wifiDateIdx[i]);

    }
    else if(menuOption == '4') {
    // LCD 16x2 - Test Code
    // Print a message to the LCD.
    lcd.setCursor(0, 0);
    lcd.print("Circuito Rocks !");
    // Turn off the display:
    lcd.noDisplay();
    delay(500);
    // Turn on the display:
    lcd.display();
    delay(500);
    }
    else if(menuOption == '5') {
    // DS3231 Precision RTC Breakout - Test Code
    //This will display the time and date of the RTC. see RTC.h for more functions such as rtcDS.hour(), rtcDS.month() etc.
    DateTime now = rtcDS.now();
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print("  ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    delay(1000);
    }
    else if(menuOption == '6') {
    // Relay Module #1 - Test Code
    // The relay will turn on and off for 500ms (0.5 sec)
    relayModule1_1.on();       // 1. turns on
    delay(500);             // 2. waits 500 milliseconds (0.5 sec). Change the value in the brackets (500) for a longer or shorter delay in milliseconds.
    relayModule1_1.off();      // 3. turns off.
    delay(500);             // 4. waits 500 milliseconds (0.5 sec). Change the value in the brackets (500) for a longer or shorter delay in milliseconds.
    }
    else if(menuOption == '7') {
    // Relay Module #2 - Test Code
    // The relay will turn on and off for 500ms (0.5 sec)
    relayModule2_2.on();       // 1. turns on
    delay(500);             // 2. waits 500 milliseconds (0.5 sec). Change the value in the brackets (500) for a longer or shorter delay in milliseconds.
    relayModule2_2.off();      // 3. turns off.
    delay(500);             // 4. waits 500 milliseconds (0.5 sec). Change the value in the brackets (500) for a longer or shorter delay in milliseconds.
    }
    
    if (millis() - time0 > timeout)
    {
        menuOption = menu();
    }
    
}



// Menu function for selecting the components to be tested
// Follow serial monitor for instrcutions
char menu()
{

    Serial.println(F("\nWhich component would you like to test?"));
    Serial.println(F("(1) Current Sensor Module ACS712"));
    Serial.println(F("(2) Piezo Buzzer"));
    Serial.println(F("(3) Logic Level Converter - Bi-Directional"));
    Serial.println(F("(4) LCD 16x2"));
    Serial.println(F("(5) DS3231 Precision RTC Breakout"));
    Serial.println(F("(6) Relay Module #1"));
    Serial.println(F("(7) Relay Module #2"));
    Serial.println(F("(menu) send anything else or press on board reset button\n"));
    while (!Serial.available());

    // Read data from serial monitor if received
    while (Serial.available()) 
    {
        char c = Serial.read();
        if (isAlphaNumeric(c)) 
        { 
            if(c == '1') 
    			Serial.println(F("Now Testing Current Sensor Module ACS712"));
    		else if(c == '2') 
    			Serial.println(F("Now Testing Piezo Buzzer"));
    		else if(c == '3') 
    			Serial.println(F("Now Testing Logic Level Converter - Bi-Directional"));
    		else if(c == '4') 
    			Serial.println(F("Now Testing LCD 16x2"));
    		else if(c == '5') 
    			Serial.println(F("Now Testing DS3231 Precision RTC Breakout"));
    		else if(c == '6') 
    			Serial.println(F("Now Testing Relay Module #1"));
    		else if(c == '7') 
    			Serial.println(F("Now Testing Relay Module #2"));
            else
            {
                Serial.println(F("illegal input!"));
                return 0;
            }
            time0 = millis();
            return c;
        }
    }
}
