#include <LiquidCrystal.h>
#include <DS3232RTC.h>
#include <EEPROMex.h>
#include <EEPROMVar.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
//pin declaration
  const int voltagesensepin = A0;
  const int ipcurrentpin = A1;
  const int opcurrentpin = A2;
  int doorsensorVal = A3;
  const int relay1 = 11;
  const int relay2 = 12;
  const int buzzer = 13;
  int master = 10; 
  int demand = 3;
    
//variable declaration  
  double voltage;
  double voltagevalue;
  double input_voltage;
  double actualinput_voltage;
  double inampslimit;
  double ipcurrentvalue ;
  double input_ipcurrent ;
  double actualinput_ipcurrent ;
  double opcurrentvalue ;
  double input_opcurrent ;
  double actualinput_opcurrent ;
  double incurrentadjsetvalue ;
  double outampslimit ;
  double outcurrentadjsetvalue ;
  double amps ;
  double totamps ;
  double avgamps ;
  double amphr ;
  const int billcost = 5;
  double energy=0;
  double power;
  double bill;
  double kwhbyte;
  int billbyte;
  double a;
  int demandvalue;

//memory declaration  
  uint16_t energyaddress = 160;
  uint16_t billvaladdress = 140;
  uint16_t metertamperaddress = 120;
  uint16_t masteraddress = 100;
  uint16_t demandaddress = 80;
  
//string to send declaration
  char watt[5];
 
void setup() 
{
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("  SMART ENERGY  ");
    lcd.setCursor(0,1);
    lcd.print("     METER      ");
    delay(1000);
    pinMode(ipcurrentpin, INPUT);
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(doorsensorVal, INPUT_PULLUP);
    pinMode(demand, INPUT_PULLUP);
    pinMode(master, INPUT_PULLUP);
    Serial.begin(115200);
    setSyncProvider(RTC.get);   
    if(timeStatus() != timeSet)
    {
     Serial.println("Unable to sync with the RTC");
     lcd.print("Unable to sync with the RTC");
    }
    else
    {
     Serial.println("RTC has set the system time");
     lcd.print("RTC has set the system time");
    }   
     EEPROM.updateDouble(energyaddress, 0);
     EEPROM.updateDouble(billvaladdress, 0);
     EEPROM.updateInt(metertamperaddress, 0);  
     EEPROM.updateInt(masteraddress, 1);
     EEPROM.updateInt(demandaddress, 1);

}

void loop() 
{ 
 doorsensorVal = digitalRead(A3);
 master = digitalRead(10);
 demand = digitalRead(3);
 if(doorsensorVal==0)
 {
 EEPROM.updateInt(metertamperaddress, 1);
 }
 if(master == 0)
 {
  EEPROM.updateInt(masteraddress, 0);
 }
 if(master == 1)
 {
  EEPROM.updateInt(masteraddress, 1); 
 }
 demand = digitalRead(3);
 if(demand == 0)
 {
  EEPROM.updateInt(demandaddress, 0);
 }
 else if( demand == 1)
 {
  EEPROM.updateInt(demandaddress, 1);
 }
           
 //ip voltage
 voltagevalue = analogRead(voltagesensepin);
 input_voltage = ((voltagevalue * 5.0) / 1024.0);
 voltage = input_voltage;
 if (input_voltage < 0.1) 
 {
  input_voltage=0.0;
 }
 actualinput_voltage = input_voltage * 60;
  
 //voltage protection
 if(actualinput_voltage < 100.0f )
 {
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);                
 }
 if(actualinput_voltage > 280.0f )
 {
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);                
 }             
 if(EEPROM.readInt(metertamperaddress) == 0)
 {
  if( EEPROM.readInt(masteraddress) == 1)
  {
    if(EEPROM.readInt(demandaddress) ==1)
    {
      if(actualinput_voltage >100.0f && actualinput_voltage < 290.0f)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);  
       }
       else
       {
          lcd.clear();
          lcd.print(" Voltage is Not ");
          lcd.setCursor(0,1);
          lcd.print("     Stable     ");      
       }
    }        
                  
    //ip current
    ipcurrentvalue = analogRead(ipcurrentpin);
    input_ipcurrent = (ipcurrentvalue * 5.0) / 1024.0;
    if (input_ipcurrent < 0.1) 
    {
     input_ipcurrent=0.0;
    } 
    actualinput_ipcurrent = input_ipcurrent * 4;

    // Demand condition
    //demand = digitalRead(3);
    if(EEPROM.readInt(demandaddress) == 0)
    {
     lcd.clear();
     lcd.print("on demand  ");
     lcd.print(demand);
     delay(500);
     digitalWrite(relay2, LOW);
     digitalWrite(relay1, HIGH);///new on 15/4/19
     if(actualinput_ipcurrent > 1.0f)
     {
      digitalWrite(buzzer, HIGH);
      delay(2000);
      digitalWrite(buzzer, LOW);
     }
     delay(4000);
     if(actualinput_ipcurrent > 1.0f)
     {
      digitalWrite(buzzer, HIGH);
      delay(2000);
      digitalWrite(buzzer, LOW);
     }
     if(actualinput_ipcurrent > 1.0f)
     {
      digitalWrite(relay1, LOW);
     } 
    }
    else
    {
     digitalWrite(relay1, HIGH);
     digitalWrite(relay2, HIGH); 
    }      
                     
    //op current multiply with 10
    opcurrentvalue = analogRead(opcurrentpin);
    input_opcurrent = (opcurrentvalue * 5.0) / 1024.0; 
    if (input_opcurrent < 0.1) 
    {
     input_opcurrent=0.0;
    }
    actualinput_opcurrent = input_opcurrent * 4;
                   
    //bill
    long milisec = millis(); // calculate time in milliseconds
    long time=milisec/1000;
    if(actualinput_ipcurrent > 0.00f)
    { 
     power = actualinput_voltage *  actualinput_ipcurrent;
     a = energy;
     energy = (power * time)/(60000);
     kwhbyte = (EEPROM.readDouble(energyaddress)) + (energy - a );
     a = energy;
     EEPROM.updateDouble(energyaddress, kwhbyte);
     dtostrf(kwhbyte, 4, 2, watt); 
     Serial.write(watt);
    }
    bill = billcost * EEPROM.readDouble(energyaddress) ;
    EEPROM.updateDouble(billvaladdress,  bill);
   digitalClockDisplay();
   delay(2000);
   lcd.clear();            
   lcd.print("I/P Volt:");
   lcd.print(actualinput_voltage );
   lcd.print("V  ");
   lcd.setCursor(0,1);
   lcd.print("I/P Curr:");
   lcd.print(actualinput_ipcurrent);
   lcd.print("A  ");
   delay(2000);
   lcd.clear();
   lcd.print("Unit : ");
   lcd.print(EEPROM.readDouble(energyaddress));
   lcd.print("KWhr");
   lcd.setCursor(0,1);
   lcd.print("Power: ");                   
   lcd.print(power);
   lcd.print("W ");
   delay(2000);      
  }                                                               
  else if(EEPROM.readInt(masteraddress) == 0   )
  {
   digitalWrite(relay1, LOW);
   digitalWrite(relay2, LOW);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("  Master close ");    
   lcd.setCursor(0,1);
   lcd.print(" contact Admin ");
   delay(1000);
  } 
 
}
else  if(EEPROM.readInt(metertamperaddress) == 1)
 {
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   case  open   ");    
  lcd.setCursor(0,1);
  lcd.print(" contact Admin ");
  delay(1000);
 }
}
void digitalClockDisplay()
{
    // digital clock display of the time
    lcd.clear();
    Serial.print(hour());
    lcd.setCursor(0,0);
    lcd.print("Time: ");
    lcd.print(hour());
    lcd.print(":");
    printDigits(minute());
    lcd.print(minute());
    lcd.print(":");
    printDigits(second());
    lcd.print(second());
    Serial.print(' ');
    lcd.print(' ');
    lcd.setCursor(0,1);
    lcd.print("Date: ");
    Serial.print(day());
    lcd.print(day());
    lcd.print("/");
    Serial.print(' ');
    Serial.print(month());
    lcd.print(month());
    lcd.print("/");
    Serial.print(year());
    lcd.print(year());
    Serial.println();
}

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
