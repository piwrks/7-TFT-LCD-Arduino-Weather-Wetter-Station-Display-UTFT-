//Arduino 1.5.6
//Wetterdisplay mit Uhrzeit

#include <Wire.h>
#include <UTFT.h> 
#include <UTFT_DLB.h>
#include <RTClib.h>
#include "Adafruit_SHT31.h"      //SHT 31 Außensensor
#include <SHT2x.h>               //SHT21 Innensensor
#include <Adafruit_BMP085.h>     //BMP085/BMP180
#include <SPI.h>
#include <SdFat.h>
#include <UTFT_SdRaw.h>
#include "sunrise.h"             //library for sunrise/sunset calculations
#include <OneWire.h>
#include <DallasTemperature.h> 

#include <DS1307.h>
#include <URTouch.h>

//RTC_DS1307 rtc; 
RTC_DS3231 rtc;

// BMP085/BMP180
Adafruit_BMP085 bmp;

//SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31(); 

 char Zeit[10]; 
 char Datum[10]; 
 char str[10]; 
 char sunbuf[61]; // variable for output formatting

//DS18B20
//digitalWrite(pin,HIGH); 
// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 43
//define TEMPERATURE_PRECISION 12
//#define TEMPERATURE_PRECISION 11
#define TEMPERATURE_PRECISION 10
//#define TEMPERATURE_PRECISION 9
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//DeviceAdressen der einzelnen ds1820 Temperatursensoren angeben.
DeviceAddress temp1 = {  0x28, 0xD0, 0xC1, 0xD0, 0x04, 0x00, 0x00, 0xAA };   
DeviceAddress temp2 = {  0x28, 0xC4, 0x6C, 0xD0, 0x04, 0x00, 0x00, 0xF2 };   
DeviceAddress temp3 = {  0x28, 0xA4, 0x7A, 0xD0, 0x04, 0x00, 0x00, 0xD1 };   
DeviceAddress temp4 = {  0x28, 0x5E, 0xD4, 0xD0, 0x04, 0x00, 0x00, 0xFD };   
DeviceAddress temp5 = {  0x28, 0xAD, 0x0F, 0xE1, 0x04, 0x00, 0x00, 0x3F };  
DeviceAddress temp6 = {  0x28, 0x17, 0xE5, 0x34, 0x05, 0x00, 0x00, 0xBB };  

//SD Karte
#define SD_CHIP_SELECT  53  // SD chip select pin
// file system object
SdFat sd;

//For Sunrise and Sunset
#define LAT 48.61  // latitude in degrees, south is negative (used in sun calculation)
#define LON 9.54  // longitude in degrees, west is negative (used in sun calculation)


// Zeitintervalle setzen
int TeilerSeriell = 1; // Serielle Ausgabe alle 1 min
int TeilerTendenz = 15;
int TeilerDisplay = 1; // Serielle Ausgabe alle 1 min



// Timer setzen
unsigned long currentMillis;
unsigned long backlightMillis;
unsigned long SeriellMillis;
long SeriellIntervall = 1000;
unsigned long DisplayMillis;
long DisplayIntervall = 1000;
unsigned long TendenzMillis;
long TendenzIntervall = 1000;



// Variablen für Werteerfassung und Umrechnung deklarieren
int altitude = 385; // Höhe des Sensorstandortes ueber dem Meeresspiegel
float MaxTemp = -1000;
float MinTemp = 1000;
int  lastDay;
float MaxTemp_Aussen = -1000;
float MinTemp_Aussen = 1000;
float MaxTemp_Innen = -1000;
float MinTemp_Innen = 1000;
float MaxHum_Aussen= -1000;
float MinHum_Aussen = 1000;
float MaxHum_Innen = -1000;
float MinHum_Innen = 1000;
float MaxLuftdruckWGT = -1000;
float MinLuftdruckWGT = 1000000;
float Temp_Aussen;
float Temp_Innen;
float Hum_Aussen;
float Hum_Innen;
float Temperatur;
float Luftdruck;
float LuftdruckWGT;
float humidex;
float dewpoint;
float atemp1;
float atemp2;
float atemp3;
float atemp4;
float atemp5;
float atemp6;
String Wetterstatus = "leer";
String Tendenz = "leer";
String Humidex_Message = "leer";
String nfm = "leer"; // days to next full moon
byte lastMinute;                                  // To know if we have a new hour and new day in this reading.
byte currentMinute = 0;                           // Value to store the current hour


// Backlight und Näherungssensor
const int backlight_pin = 44; // ON CTE SHIELD JP5 Closed/Soldered. JP3 and JP4 open.
const int backlight_on = 255;
const int backlight_off = 0;
const int  STATE = 49;                 // sets pin 49 for sensor reading
int r_state = 0;                      // reset to zero the variable used to read the state of the OUT pin of the sensor


// Declare which fonts we will be using
extern uint8_t SmallFont[];
//extern uint8_t BigFont[];
//extern uint8_t SevenSegNumFont[];
//extern uint8_t DejaVuSans18[];
//extern uint8_t DejaVuSans24[];
//extern uint8_t meteocons38[];
extern uint8_t BVS_9[];
extern uint8_t BVS_7[];
extern uint8_t BVS_11[];
extern uint8_t BVS_13[];
extern uint8_t BVS_15[];
extern uint8_t BVS_19[];
extern uint8_t BVS_22[];
extern uint8_t BVS_28[];
extern uint8_t BVS_34[];
extern uint8_t BVS_43[];


// Using Arduino Mega and 7 TFT LCD SSD1963 800x480 Display
//UTFT myGLCD(CTE70,38,39,40,41);
UTFT_DLB myGLCD(CTE70,38,39,40,41);

//Touch
URTouch  myTouch(6,5,4,3,2);

//Variable für Zeitstellmenue

Time  t;
DS1307 rtc2(20, 21);

// Variablen für Tendenz P-Zeit in Minuten
float Pnow;
float P15;
float P30;
float P45;
float P60; //1h
float P75;
float P90;
float P105;
float P120; //2h
float P135;
float P150;
float P165;
float P180; //3h

UTFT_SdRaw myFiles(&myGLCD);

void setup() 
{ 
 Serial.begin(9600); 
 rtc.begin();            //RTC starten
 Wire.begin();
 bmp.begin();            // BMP starten
 sht31.begin(0x44);      // SHT31 starten
 sensors.begin();        // DS18B20 starten
 
 rtc2.begin();         //RTC Menue
  // Set the clock to run-mode
 rtc2.halt(false);   //RTC Menue
 
sensors.setResolution(temp1, TEMPERATURE_PRECISION);
sensors.setResolution(temp2, TEMPERATURE_PRECISION);
sensors.setResolution(temp3, TEMPERATURE_PRECISION);
sensors.setResolution(temp4, TEMPERATURE_PRECISION);
sensors.setResolution(temp5, TEMPERATURE_PRECISION);
sensors.setResolution(temp6, TEMPERATURE_PRECISION);

pinMode (backlight_pin, OUTPUT);      // sets pin 44 as analog output
pinMode (STATE, INPUT);              // sets pin 49 as digital input 
 
 Serial.println(F("Initialising SD card..."));
  bool mysd = 0;
  // see if the card is present and can be initialized:
  while (!mysd)
  {
    if (!sd.begin(SD_CHIP_SELECT, SPI_FULL_SPEED)) {
      Serial.println(F("Card failed, or not present"));
      Serial.println(F("Retrying...."));
    }
    else
    {
      mysd = 1;
      Serial.println(F("Card initialised."));
    }
     }
     
initGraphics();
 
 //rtc.adjust(DateTime(__DATE__, __TIME__)); // EINMALIG: kalibrieren der Uhr mit PC 

 
 // Druck für Tendenz Startwert in alle Variablen schreiben
 Luftdruck = bmp.readPressure(); // Erfasse Luftdruck
 LuftdruckWGT = bmp.readSealevelPressure(altitude)/100.0F; 
 Pnow = LuftdruckWGT;
 P15 = LuftdruckWGT;
 P30 = LuftdruckWGT;
 P45 = LuftdruckWGT;
 P60 = LuftdruckWGT; //1h
 P75 = LuftdruckWGT;
 P90 = LuftdruckWGT;
 P105 = LuftdruckWGT;
 P120 = LuftdruckWGT; //2h
 P135 = LuftdruckWGT;
 P150 = LuftdruckWGT;
 P165 = LuftdruckWGT;
 P180 = LuftdruckWGT; //3h
 Tendenz = "Erhebe Daten";
 
}


void loop() 
{
 
  
 //Menue für Uhrzeit
 
  int x, y;
  t = rtc2.getTime();

    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if (((y>=1) && (y<=40)) && ((x>=1) && (x<=795)))
      {
        myGLCD.setColor (255, 0, 0);
       myGLCD.drawRoundRect(1, 1, 795, 40);
        setClock();
          }
   }
 
 
  // Teil 1 Berechnung aller Werte...
 // 
  
 Luftdruck = bmp.readPressure(); // Erfasse Luftdruck
 //Temperatur = bmp.readTemperature(); // Erfasse Temperatur
 Temp_Aussen = sht31.readTemperature();
 Temp_Innen = SHT2x.GetTemperature();
 Hum_Aussen = sht31.readHumidity();
 Hum_Innen = SHT2x.GetHumidity();
 LuftdruckWGT = bmp.readSealevelPressure(altitude)/100.0F; 
 humidex = calculate_humidex (Temp_Aussen, Hum_Aussen); //Aussensensor
 dewpoint = dewPoint(Temp_Innen, Hum_Innen); //Innensensor 
 // themperatures from all sensors DS18B20
 sensors.requestTemperatures();
 // save themperatures to values DS18B20
atemp1 = sensors.getTempC(temp1);
atemp2 = sensors.getTempC(temp2);
atemp3 = sensors.getTempC(temp3);
atemp4 = sensors.getTempC(temp4);
atemp5 = sensors.getTempC(temp5);
atemp6 = sensors.getTempC(temp6);
 
 
 if(Temp_Aussen < MinTemp_Aussen) {MinTemp_Aussen = Temp_Aussen;}
 if(Temp_Aussen > MaxTemp_Aussen) {MaxTemp_Aussen = Temp_Aussen;}
 
 if(Temp_Innen < MinTemp_Innen) {MinTemp_Innen = Temp_Innen;}
 if(Temp_Innen > MaxTemp_Innen) {MaxTemp_Innen = Temp_Innen;}
 
 if(Hum_Aussen < MinHum_Aussen) {MinHum_Aussen = Hum_Aussen;}
 if(Hum_Aussen > MaxHum_Aussen) {MaxHum_Aussen = Hum_Aussen;}
 
 if(Hum_Innen < MinHum_Innen) {MinHum_Innen = Hum_Innen;}
 if(Hum_Innen > MaxHum_Innen) {MaxHum_Innen = Hum_Innen;}
 
 if(LuftdruckWGT < MinLuftdruckWGT) {MinLuftdruckWGT = LuftdruckWGT;}
 if(LuftdruckWGT > MaxLuftdruckWGT) {MaxLuftdruckWGT = LuftdruckWGT;}
  
 
 // WetterAktuell:
 
 if ((LuftdruckWGT <= 980)&& (Temp_Aussen >= 0)){Wetterstatus = "Sturm, Regen";}
 if ((LuftdruckWGT <= 980)&& (Temp_Aussen < 0)){Wetterstatus = "Sturm, Schnee";}
 if ((LuftdruckWGT > 980) && (LuftdruckWGT <= 1000) && (Temp_Aussen >= 0)) {Wetterstatus = "Regnerisch";} 
 if ((LuftdruckWGT > 980) && (LuftdruckWGT <= 1000) && (Temp_Aussen < 0)) {Wetterstatus = "Schneeschauer";} 
 if ((LuftdruckWGT > 1000) && (LuftdruckWGT <= 1020)){Wetterstatus = "Wechselhaft";} 
 if ((LuftdruckWGT > 1020) && (LuftdruckWGT <= 1040)){Wetterstatus = "Sonnig,bestaendig";} 
 if (LuftdruckWGT > 1040){Wetterstatus = "Trocken,Gewitter";} 
  
 //Alternative Werte
 //970 bis 987, Sturm
 //988 bis 1001, Regen
 //1002 bis 1026, wechselhaft
 //1027 bis 1042, schön
 //1043 bis 1060, trocken
 
 currentMillis = millis(); // Timer für jeden Durchgang setzten.
 
 //Serial.println(currentMillis);
 DateTime aktuell = rtc.now();
 
 // Teil 2 Tendenzen berechen..
 //
 
 if ((aktuell.second() >= 0 ) && (aktuell.second() <= 3 ) && (aktuell.minute()%TeilerTendenz == 0)) // Wenn die aktuelle Minute durch 15 teilbar ist, dann.... 
 {
 if(currentMillis - TendenzMillis > TendenzIntervall)
 { 
 // Register eins weiterschieben
 
Serial.println("Register eins weiterschieben");
 
 P180 = P165;
 P165 = P150;
 P150 = P135;
 P135 = P120;
 P120 = P105;
 P105 = P90;
 P90 = P75;
 P75 = P60;
 P60 = P45;
 P45 = P30;
 P30 = P15;
 P15 = Pnow;
 Pnow = LuftdruckWGT; 

 
 // Tendenz berechnen
 if( Pnow-P180 <= -8 ){Tendenz ="Sturm mit Hagel"; } 
 if((Pnow-P180 <= -5 ) && (Pnow-P180 > -8 )){Tendenz ="Regen/Unwetter";} 
 if((Pnow-P180 <= -3 ) && (Pnow-P180 > -5 )){Tendenz ="Regnerisch";} 
 if((Pnow-P180 <= -1.3) && (Pnow-P180 > -3 )){Tendenz ="baldiger Regen";} 
 if((Pnow-P180 <= 1.3 ) && (Pnow-P180 > -1.3)){Tendenz ="gleichbleibend";} 
 if((Pnow-P180 <= 3 ) && (Pnow-P180 >= 1.3)){Tendenz ="lange Schoen";}
 if((Pnow-P180 <= 5 ) && (Pnow-P180 >= 3 )){Tendenz ="Schoen & labil";}
 if( Pnow-P180 > 5 ){Tendenz = "Sturmwarnung";}
 
  // Tendenz Alternativ berechnen
 //if( Pnow-P180 <= -8 ){Tendenz ="Sturm mit Hagel";} 
 //if((Pnow-P180 <= -5 ) && (Pnow-P180 > -8 )){Tendenz ="Regen/Unwetter";} 
 //if((Pnow-P180 <= -3 ) && (Pnow-P180 > -5 )){Tendenz ="Regnerisch";} 
 //if((Pnow-P180 <= -0.5) && (Pnow-P180 > -3 )){Tendenz ="baldiger Regen";} 
 //if((Pnow-P180 <= 0.5 ) && (Pnow-P180 > -0.5)){Tendenz ="gleichbleibend";} 
 //if((Pnow-P180 <= 3 ) && (Pnow-P180 >= 0.5)){Tendenz ="lange Schoen";}
 //if((Pnow-P180 <= 5 ) && (Pnow-P180 >= 3 )){Tendenz ="Schoen & labil";}
 //if( Pnow-P180 > 5 ){Tendenz = "Sturmwarnung";}
  
 TendenzMillis = currentMillis;
 //Serial.println(TendenzMillis);
 }
 } 
 
 
 // Teil 3 Humudex_Meldung

if ((humidex >= 21 )&&(humidex < 27))
  {
    Humidex_Message= "Angenehm"; //No discomfort
  } 
  
  if ((humidex >= 27 )&&(humidex < 35))
  {
    Humidex_Message= "Leichtes Unbehagen";  //Some discomfort
  }

  if ((humidex >= 35 )&&(humidex < 40))
  {
    Humidex_Message= "Starkes Unbehagen";  //Great discomfort
  } 

  if ((humidex >= 40 )&&(humidex < 46))
  {
    Humidex_Message= "Gesundheitsgefahr";  //Health risk
  } 

  if ((humidex >= 46 )&&(humidex < 54))
  {
    Humidex_Message= "Große Gesundheitsgefahr";  //Great health risk
  } 

  if ((humidex >= 54 ))
  {
    Humidex_Message= "Hitzschlagsgefahr";  //Heat stroke danger
  } 
  
 
 // Teil 4 Ausgabe Serielle Schnittstelle

 if ((aktuell.second() >= 0 ) && (aktuell.second() <= 3 ) && (aktuell.minute()%TeilerSeriell == 0))
 {
 if(currentMillis - SeriellMillis > SeriellIntervall)
 {
 if (aktuell.day() < 10){Serial.print(F("0"));} // EXCEL DATUM: "TT.MM.JJJJ HH:MM:SS"
 Serial.print(aktuell.day());
 Serial.print(F("."));
 if (aktuell.month() < 10){Serial.print(F("0"));}
 Serial.print(aktuell.month());
 Serial.print(F("."));
 Serial.print(aktuell.year());
 Serial.print(F(" ")); 
 if (aktuell.hour() < 10){Serial.print(F("0"));}
 Serial.print(aktuell.hour());
 Serial.print(F(":"));
 if (aktuell.minute() < 10){Serial.print(F("0"));}
 Serial.print(aktuell.minute());
 Serial.print(F(":"));
 if (aktuell.second() < 10){Serial.print(F("0"));} 
 Serial.print(aktuell.second());
 Serial.println(F(" "));
 
 Serial.print(F("Luftdruck"));
 Serial.print(F(" ")); 
 Serial.println(LuftdruckWGT); 
 Serial.print(F("Delta 3h"));
 Serial.print(F(" ")); 
 Serial.println(Pnow-P180); 
 Serial.print(F("Delta 1h"));
 Serial.print(F(" ")); 
 Serial.println(Pnow-P90);
 Serial.print(F("Wetterstatus"));
 Serial.print(F(" ")); 
 Serial.println(Wetterstatus); 
 Serial.print(F("Tendenz"));
 Serial.print(F(" ")); 
 Serial.println(Tendenz); // println für neue Zeile 
 Serial.print(F("Temp_Aussen"));
 Serial.print(F(" ")); 
 Serial.println(Temp_Aussen); 
 Serial.print(F("Temp_Innen"));
 Serial.print(F(" ")); 
 Serial.println(Temp_Innen); 
 Serial.print(F("Hum_Aussen"));
 Serial.print(F(" ")); 
 Serial.println(Hum_Aussen); 
 Serial.print(F("Hum_Innen")); 
 Serial.print(F(" ")); 
 Serial.println(Hum_Innen); 
 Serial.print(F("Humidex Aussen"));
 Serial.print(F(" ")); 
 Serial.println(humidex);
 Serial.print(F("Taupunkt Innen")); 
 Serial.print(F(" ")); 
 Serial.println(dewpoint);
 Serial.print(F("Humidex"));
 Serial.print(F(" ")); 
 Serial.println(Humidex_Message);
 Serial.print(F("MinTemp_Aussen"));
 Serial.print(F(" ")); 
 Serial.println(MinTemp_Aussen);
 Serial.print(F("MaxTemp_Aussen"));
 Serial.print(F(" ")); 
 Serial.println(MaxTemp_Aussen); 
 Serial.print(F("MinTemp_Innen"));
 Serial.print(F(" ")); 
 Serial.println(MinTemp_Innen); 
 Serial.print(F("MaxTemp_Innen"));
 Serial.print(F(" ")); 
 Serial.println(MaxTemp_Innen); 
 Serial.print(F("MinHum_Aussen"));
 Serial.print(F(" ")); 
 Serial.println(MinHum_Aussen); 
 Serial.print(F("MaxHum_Aussen"));
 Serial.print(F(" ")); 
 Serial.println(MaxHum_Aussen);
 Serial.print(F("MinHum_Innen")); 
 Serial.print(F(" ")); 
 Serial.println(MinHum_Innen); 
 Serial.print(F("MaxHum_Innen")); 
 Serial.print(F(" ")); 
 Serial.println(MaxHum_Innen); 
 Serial.print(F("MinLuftdruckWGT")); 
 Serial.print(F(" ")); 
 Serial.println(MinLuftdruckWGT); 
 Serial.print(F("MaxLuftdruckWGT")); 
 Serial.print(F(" ")); 
 Serial.println(MaxLuftdruckWGT); 
 Serial.print(F("1-Wire Nr. 1")); 
 Serial.print(F(" ")); 
 Serial.println(atemp1); 
  Serial.print(F("1-Wire Nr. 2")); 
 Serial.print(F(" ")); 
 Serial.println(atemp2); 
  Serial.print(F("1-Wire Nr. 3")); 
 Serial.print(F(" ")); 
 Serial.println(atemp3); 
  Serial.print(F("1-Wire Nr. 4")); 
 Serial.print(F(" ")); 
 Serial.println(atemp4); 
  Serial.print(F("1-Wire Nr. 5")); 
 Serial.print(F(" ")); 
 Serial.println(atemp5); 
  Serial.print(F("1-Wire Nr. 6")); 
 Serial.print(F(" ")); 
 Serial.println(atemp6); 
                                                               
  
 SeriellMillis = currentMillis;
 //Serial.println(SeriellMillis);
 }
 }
 
  // Teil 5 Reset Min/Max
  
  //Min-Max Werte alle 24 Stunden um Mitternacht resetten
//DateTime now; 
//Lets see what time the RTC is set at! -- If RTC is used
 DateTime reset = rtc.now();

 //Serial.println(reset.day());
 //Serial.println(lastDay);
 
//if (now.day() != lastDay) // this happens exactly once a day.
if(reset.day() != lastDay) 
{ 
MaxTemp_Aussen = Temp_Aussen;
MinTemp_Aussen = Temp_Aussen;
MaxTemp_Innen = Temp_Innen;
MinTemp_Innen = Temp_Innen;
MaxHum_Aussen= Hum_Aussen;
MinHum_Aussen = Hum_Aussen;
MaxHum_Innen = Hum_Innen;
MinHum_Innen = Hum_Innen;
MaxLuftdruckWGT = LuftdruckWGT;
MinLuftdruckWGT = LuftdruckWGT;
lastDay = reset.day(); }

 //Serial.println(lastDay);    
 
 //Alternative:
  //    int last_hour = hour;
   // hour = now.hour();
   // minute = now.minute();
   // if (last_hour == 23 && hour == 00) 
  //  { 
 //     int i; 
  //    genkwh1 = 0;
  //   genkwh2 = 0;
  //   usekwh = 0;
  //    mintemp = inttemp;
 //    maxtemp = inttemp;
//   }
 
   // Teil 6  Display Ausgabe 
   //Evtl. über uhr nur jede minute aktualisieren lassen wie bei Druckmessung
   
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(0, 14, 799, 14); // Datum
  renderDatum();
  
  if ((aktuell.second() >= 0 ) && (aktuell.second() <= 3 ) && (aktuell.minute()%TeilerDisplay == 0)) 
 {
 if(currentMillis - DisplayMillis > DisplayIntervall)
 { 
  allesleeren();
  renderTempInnen();
  renderTempAussen();
  renderBarometer();
  rendertaupunkt();
  renderhumidex();
  renderaktuell();
  renderforecast();
  rendermoon();
  rendermoondays();
  rendersonne();
  renderheizung();
  
  
 DisplayMillis = currentMillis;
 //Serial.println(SeriellMillis);
 }
 }

//Bargraphen

//addHistoryValueBaro(LuftdruckWGT);

DateTime Bargraph = rtc.now();
currentMinute = Bargraph.minute();

     if (currentMinute == 0 && lastMinute == 59)
    { //means currentMinute = 0 and lastMinute = 59
      addHistoryValueBaro(LuftdruckWGT);
       }  
 
  lastMinute = currentMinute;
    
     
 // Backlight Steuerung mit SHARP GP2Y0D810Z0F 
 
 r_state = digitalRead(STATE); // reads the status of the sensor
if(r_state == 0)              // if is there an obstacle (OUT = 0)
           {  analogWrite(backlight_pin, backlight_on);   // turn on the LCD
            backlightMillis = currentMillis;}
    
    
       if(currentMillis - backlightMillis > 60000) // 60 Sekunden
       {  analogWrite(backlight_pin, backlight_off);    // turn off the LCD}
    }
     
 
} //ENDE LOOP



//******************* GRAPHICS INIT ***************************
void initGraphics() {
  
  analogWrite(backlight_pin, backlight_on); 
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  
  myTouch.InitTouch();
  //myTouch.setPrecision(PREC_LOW);
  //myTouch.setPrecision(PREC_MEDIUM);
  myTouch.setPrecision(PREC_HI);
  //myTouch.setPrecision(PREC_EXTREME);
  
  //myGLCD.fillScr(0,0,0);
  //myGLCD.setBackColor(240,240,240);
  //myGLCD.setColor(0, 255, 0);
  
  myGLCD.clrScr();
    
  //Beschriftung und Linien hinzufügen
    
 myGLCD.setBackColor(0, 0, 0);
 myGLCD.setColor(255, 255, 255);
 myGLCD.drawLine(0, 15, 799, 15);
 myGLCD.drawLine(410, 15, 410, 480);
 myGLCD.drawLine(0, 290, 410, 290);
 myGLCD.drawLine(200, 15, 200, 480);
 myGLCD.drawLine(410, 215, 799, 215);
 myGLCD.drawLine(600, 335, 600, 480);
 myGLCD.drawLine(410, 335, 600, 335);
 
 myGLCD.setFont(BVS_15);
 
 myGLCD.print("Innen - Klima", 50, 30, 0);
 myGLCD.print("Grad C", 70, 105, 0);
 myGLCD.print("Feuchte %", 50, 185, 0);
   
 myGLCD.print("Aussen - Klima", 250, 30, 0);
 myGLCD.print("Grad C", 270, 105, 0);
 myGLCD.print("Feuchte %", 250, 185, 0);
 
 myGLCD.print("Barometer", 550, 230, 0);
 myGLCD.print("hPa", 660, 305, 0); 
 
 myGLCD.setFont(BVS_9); 
 myGLCD.print("Tendenz hPa 1-Std", 650, 385, 0);   
 myGLCD.print("Tendenz hPa 3-Std", 650, 455, 0);  

 myGLCD.setFont(BVS_15);
 myGLCD.print("Taupunkt", 48, 265, 0);
 myGLCD.print("Aktuell:", 480, 30, 0);
 myGLCD.print("Vorhersage:", 645, 30, 0);
 
 myGLCD.print("Mondphase", 255, 310, 0);
 
 myGLCD.print("Heizung", 475, 350, 0);
 
 myGLCD.setFont(BVS_9); 
 myGLCD.print("Puffer oben", 420, 380, 0);
 myGLCD.print("Puffer mitte", 420, 395, 0); 
 myGLCD.print("Vorlauf", 420, 410, 0);
 myGLCD.print("Ruecklauf", 420, 425, 0); 
 myGLCD.print("Kessel", 420, 440, 0);
 myGLCD.print("Schlafzimmer", 420, 455, 0); 
 myGLCD.print(":", 520, 380, 0);
 myGLCD.print(":", 520, 395, 0); 
 myGLCD.print(":", 520, 410, 0);
 myGLCD.print(":", 520, 425, 0); 
 myGLCD.print(":", 520, 440, 0);
 myGLCD.print(":", 520, 455, 0);
  
}
//Clear all
  void allesleeren() { 
 
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(0, 50, 190, 100); // Innen
  myGLCD.fillRect(0, 120, 190, 184);  // Innen
  myGLCD.fillRect(0, 200, 190, 264);  // Innen
  myGLCD.fillRect(205, 50, 408, 100); // Aussen
  myGLCD.fillRect(205, 120, 408, 184); // Aussen
  myGLCD.fillRect(578, 255, 799, 302); // Barometer
  myGLCD.fillRect(620, 345, 770, 384); // Barometer
  myGLCD.fillRect(620, 415, 770, 454); // Barometer
  myGLCD.fillRect(0, 200, 150, 264); // Taupunkt
  myGLCD.fillRect(205, 200, 408, 280); // Humidex
  myGLCD.fillRect(412, 50, 604, 210); // Vorhersage
  myGLCD.fillRect(605, 50, 799, 210); // Wetter aktuell
  myGLCD.fillRect(205, 430, 408, 460); // Mond  
  myGLCD.fillRect(335, 340, 370, 370); // Mond 
  myGLCD.fillRect(25, 300, 100, 325); // Sonne
  myGLCD.fillRect(25, 390, 100, 420); // Sonne
  myGLCD.fillRect(525, 375, 590, 470); // Heizung
  
 }
  
   void renderDatum() {  
      
  DateTime render = rtc.now();
   
  myGLCD.setBackColor(0,0,0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(SmallFont);
  
  //Uhrzeit 
 sprintf(Zeit,"%02d:%02d:%02d",render.hour(),render.minute(),render.second());   
 myGLCD.print((Zeit), 730, 1, 0); 
   
 //Datum
 sprintf(Datum,"%02d.%02d.%d",render.day(),render.month(),render.year());   
 myGLCD.print((Datum), 20, 1, 0); }
 
 void renderTempInnen() { 
 
 //Innen 
 
  
  myGLCD.setColor(255, 255, 255);
  
 myGLCD.setBackColor(0,0,0);
 myGLCD.setFont(BVS_43); 
  
 myGLCD.print(dtostrf(Temp_Innen, 5, 1, str), 20, 60, 0);
 myGLCD.print(dtostrf(Hum_Innen, 5, 1, str), 20, 140, 0);
 myGLCD.setFont(BVS_13);
 myGLCD.setColor(255, 0, 0);
 myGLCD.print(dtostrf(MaxTemp_Innen, 5, 1, str), 135, 60, 0);
 myGLCD.print(dtostrf(MaxHum_Innen, 5, 1, str), 135, 140, 0);
 myGLCD.setColor(0, 0, 255);
 myGLCD.print(dtostrf(MinTemp_Innen, 5, 1, str), 135, 82, 0);
 myGLCD.print(dtostrf(MinHum_Innen, 5, 1, str), 135, 162, 0);
 }
 
  void renderTempAussen() { 
 
 //Aussen
  
 
 myGLCD.setFont(BVS_43); 
 myGLCD.setColor(255, 255, 255);
 myGLCD.print(dtostrf(Temp_Aussen, 5, 1, str), 230, 60, 0);
 myGLCD.print(dtostrf(Hum_Aussen, 5, 1, str), 230, 140, 0);
  myGLCD.setFont(BVS_13);
 myGLCD.setColor(255, 0, 0);
 myGLCD.print(dtostrf(MaxTemp_Aussen, 5, 1, str), 345, 60, 0);
 myGLCD.print(dtostrf(MaxHum_Aussen, 5, 1, str), 345, 140, 0);
 myGLCD.setColor(0, 0, 255);
 myGLCD.print(dtostrf(MinTemp_Aussen, 5, 1, str), 345, 82, 0);
 myGLCD.print(dtostrf(MinHum_Aussen, 5, 1, str), 345, 162, 0);
 
 }
  
 void renderBarometer() {  
  
 //Barometer
 
   
 myGLCD.setFont(BVS_43); 
 myGLCD.setColor(255, 255, 255);
 myGLCD.print(dtostrf(LuftdruckWGT, 5, 1, str), 580, 260, 0);
 myGLCD.setFont(BVS_13);
 myGLCD.setColor(255, 0, 0);
 myGLCD.print(dtostrf(MaxLuftdruckWGT, 5, 1, str), 745, 260, 0);
 myGLCD.setColor(0, 0, 255);
 myGLCD.print(dtostrf(MinLuftdruckWGT, 5, 1, str), 745, 282, 0);
 myGLCD.setFont(BVS_43); 
 myGLCD.setColor(255, 255, 255);
 myGLCD.setFont(BVS_34); 
 myGLCD.print(dtostrf(Pnow-P90, 5, 1, str), 650, 350, 0);
 myGLCD.print(dtostrf(Pnow-P180, 5, 1, str), 650, 420, 0);
 }
 
  void rendertaupunkt() {  
 
 //Taupunkt
 
  
 myGLCD.setFont(BVS_43); 
 myGLCD.setColor(255, 255, 255);
 myGLCD.print(dtostrf(dewpoint, 5, 1, str), 20, 220, 0);
 }
 
  void renderhumidex() {  
 
 //Humidex mit Message
 
 
 myGLCD.setFont(BVS_43); 
 myGLCD.setColor(255, 255, 255);
 myGLCD.print(dtostrf(humidex, 5, 1, str), 230, 220, 0);
 myGLCD.setFont(BVS_15);
 if ((humidex >= 21 )&&(humidex < 27))
  {  myGLCD.setColor(0, 255, 0);
     myGLCD.print("Angenehm", 250, 265, 0); //No discomfort      
  } 
  if ((humidex >= 27 )&&(humidex < 35))
  { myGLCD.setColor(255, 192, 64);
    myGLCD.print("Leichtes Unbehagen", 220, 265, 0);   //Some discomfort
  }
  if ((humidex >= 35 )&&(humidex < 40))
  { myGLCD.setColor(255, 64, 0);
   myGLCD.print("Starkes Unbehagen", 215, 265, 0);  //Great discomfort
  } 
  if ((humidex >= 40 )&&(humidex < 46))
  { myGLCD.setColor(255, 0, 0);
  myGLCD.print("Gesundheitsgefahr", 215, 265, 0);  //Health risk
  } 
  if ((humidex >= 46 )&&(humidex < 54))
  { myGLCD.setColor(255, 0, 0);
  myGLCD.print("Gr. Gesundheitsgefahr", 210, 265, 0);  //Great health risk
  } 
  if ((humidex >= 54 ))
  { myGLCD.setColor(255, 0, 0);
    myGLCD.print("Hitzschlagsgefahr", 225, 265, 0); //Heat stroke danger
  } 
  
  }
  
    void renderforecast() {  
 
  // Wettertendenz/Wettervorhersage ausgeben
  
   
  
 if( Pnow-P180 <= -8 ){ myGLCD.setColor(255, 0, 0);
        myFiles.load(625, 48, 140, 140, "tstorms.raw", 1 , 0);    //t-Storms
       myGLCD.print("Sturm mit Hagel", 625, 190, 0);
     
 } 
 if((Pnow-P180 <= -5 ) && (Pnow-P180 > -8 )){ myGLCD.setColor(0, 255, 0);
    myFiles.load(625, 48, 140, 140, "rain_with_wind.raw", 1 , 0);   //Rain with wind
      myGLCD.print("Regen/Unwetter", 625, 190, 0);
     
 }  
 if((Pnow-P180 <= -3 ) && (Pnow-P180 > -5 )){ myGLCD.setColor(0, 255, 0);
 myFiles.load(625, 48, 140, 140, "rain.raw", 1 , 0);  //rain
     myGLCD.print("regnerisch", 650, 190, 0);
   
 }  
 if((Pnow-P180 <= -1.3) && (Pnow-P180 > -3 )){ myGLCD.setColor(0, 255, 0);
 myFiles.load(625, 48, 140, 140, "cloudy.raw", 1 , 0);  //Cloudy
     myGLCD.print("baldiger Regen", 630, 190, 0);
  
 } 
 if((Pnow-P180 <= 1.3 ) && (Pnow-P180 > -1.3)){ myGLCD.setColor(0, 255, 0);
   myFiles.load(625, 48, 140, 140, "equal_sign.raw", 1 , 0); //Equal Sign
   myGLCD.print("gleichbleibend", 640, 190, 0);
  
 } 
 if((Pnow-P180 <= 3 ) && (Pnow-P180 >= 1.3)){ myGLCD.setColor(0, 255, 0);
  myFiles.load(625, 48, 140, 140, "sun.raw", 1 , 0); //sun
     myGLCD.print("lange schoen", 645, 190, 0);
   
 } 
 if((Pnow-P180 <= 5 ) && (Pnow-P180 >= 3 )){ myGLCD.setColor(0, 255, 0);
 myFiles.load(625, 48, 140, 140, "partly_cloudy_with_sun.raw", 1 , 0);    //partly cloudy with sun
     myGLCD.print("schoen und labil", 640, 190, 0);

 } 
 if( Pnow-P180 > 5 ){ myGLCD.setColor(255, 0, 0);
    myFiles.load(625, 48, 140, 140, "wind.raw", 1 , 0); //wind
     myGLCD.print("Sturmwarnung", 635, 190, 0);

  
 } 
 
 }
     
      void renderaktuell() {   
     
    
  // WetterAktuell
  
 if ((LuftdruckWGT <= 980)&& (Temp_Aussen >= 0)){ myGLCD.setColor(0, 255, 0);
   myFiles.load(435, 48, 140, 140, "heavy_rain_with_sun.raw", 1 , 0); //heavyrain with sun
     myGLCD.print("Sturm, Regen", 450, 190, 0);   
 }  
 if ((LuftdruckWGT <= 980)&& (Temp_Aussen < 0)){ myGLCD.setColor(0, 255, 0);
  myFiles.load(435, 48, 140, 140, "heavy_snow_with_sun.raw", 1 , 0);//heavysnow with sun 
     myGLCD.print("Sturm, Schnee", 450, 190, 0);
  }  
 if ((LuftdruckWGT > 980) && (LuftdruckWGT <= 1000) && (Temp_Aussen >= 0)) { myGLCD.setColor(0, 255, 0);
   myFiles.load(435, 48, 140, 140, "light_rain_with_sun.raw", 1 , 0);  //lightrain with sun
     myGLCD.print("regnerisch", 465, 190, 0);
    
 }  
 if ((LuftdruckWGT > 980) && (LuftdruckWGT <= 1000) && (Temp_Aussen < 0)) { myGLCD.setColor(0, 255, 0);
  myFiles.load(435, 48, 140, 140, "light_snow_with_sun.raw", 1 , 0);  //lightsnow with sun
     myGLCD.print("Schneeschauer", 450, 190, 0);
   
 }  
 if ((LuftdruckWGT > 1000) && (LuftdruckWGT <= 1020)){ myGLCD.setColor(0, 255, 0);
   myFiles.load(435, 48, 140, 140, "partly_cloudy_with_sun.raw", 1 , 0); //partly cloudy with sun
     myGLCD.print("wechselhaft", 465, 190, 0); 
       
    }  
 if ((LuftdruckWGT > 1020) && (LuftdruckWGT <= 1040)){ myGLCD.setColor(0, 255, 0);
      myFiles.load(435, 48, 140, 140, "fair_day.raw", 1 , 0);  //fairday
     myGLCD.print("sonnig, bestaendig", 430, 190, 0);
  
 }   
 if (LuftdruckWGT > 1040){ myGLCD.setColor(0, 255, 0);
       myFiles.load(435, 48, 140, 140, "sun.raw", 1 , 0); //sun
     myGLCD.print("trocken, Gewitter", 435, 190, 0);
   
 }   
         
 //Alternative Werte
 //970 bis 987, Sturm
 //988 bis 1001, Regen
 //1002 bis 1026, wechselhaft
 //1027 bis 1042, schön
 //1043 bis 1060, trocken
 
 }
  
  void rendermoon() { 
  
  //Mond
   
   DateTime moon = rtc.now();
   int nYear = moon.year();
   int nMonth = moon.month();
   int nDay = moon.day();
   int nHour = moon.hour();
      int phase;
    double AG, IP; 
    long YY, MM, K1, K2, K3, JD;
    YY = nYear - floor((12 - nMonth) / 10);
    MM = nMonth + 9;
    if (MM >= 12){
      MM = MM - 12;
    }
    K1 = floor(365.25 * (YY + 4712));
    K2 = floor(30.6 * MM + 0.5);
    K3 = floor(floor((YY / 100) + 49) * 0.75) - 38;
    JD = K1 + K2 + nDay + 59;
    if (JD > 2299160){
      JD = JD - K3;
      }
  
    IP = MyNormalize((JD - 2451550.1) / 29.530588853);
    AG = IP*29.53;
   
   phase = 0;
    
  myGLCD.setFont(BVS_15);
  myGLCD.setColor(0, 255, 0);  
    
    if (AG < 1.84566) 
  {
    phase = 0;
   // myGLCD.drawBitmap (400, 50, 60, 60, new_moon);
      myGLCD.print("Neumond", 260, 435, 0);
    myFiles.load(270, 350, 60, 60, "newmoon.raw", 1 , 0);
  }
  else if (AG < 5.53699)
  {
    phase = 1;
  // myGLCD.drawBitmap (400, 50, 60, 60, waxing_crescent);
        myGLCD.print("Sichelmond zunehmend", 210, 435, 0);
     myFiles.load(270, 350, 60, 60, "waxingcrescent.raw", 1 , 0);
  }
  else if (AG < 9.922831)
  {
    phase = 2;
  //   myGLCD.drawBitmap (400, 50, 60, 60, first_quarter);
        myGLCD.print("Halbmond zunehmend", 215, 435, 0);
      myFiles.load(270, 350, 60, 60, "firstquarter.raw", 1 , 0);
  }
  else if (AG < 12.91963)
  {
    phase = 3;
  //  myGLCD.drawBitmap (400, 50, 60, 60, waxing_gibbous);
       myGLCD.print("Vollmond zunehmend", 220, 435, 0);
     myFiles.load(270, 350, 60, 60, "waxinggibbous.raw", 1 , 0);
   
  }
  else if (AG < 16.61096)
  {
    phase = 4;
    //myGLCD.drawBitmap (50, 400, 60, 60, full);
         myGLCD.print("Vollmond", 265, 435, 0);
     myFiles.load(270, 350, 60, 60, "full.raw", 1 , 0);
  }
  else if (AG < 20.30228)
  {
    phase = 5;
   // myGLCD.drawBitmap (400, 50, 60, 60, waning_gibbous);
       myGLCD.print("Vollmond abnehmend", 220, 435, 0);
     myFiles.load(270, 350, 60, 60, "waninggibbous.raw", 1 , 0);
  }
  else if (AG < 23.99361)
  {
    phase = 6;
   //  myGLCD.drawBitmap (400, 50, 60, 60, last_quarter);
   myGLCD.print("Halbmond abnehmend", 215, 435, 0);
    myFiles.load(270, 350, 60, 60, "lastquarter.raw", 1 , 0);
  }
  else if (AG < 27.68493)
  {
    phase = 7;
    // myGLCD.drawBitmap (400, 50, 60, 60, waning_crescent);
    myGLCD.print("Sichelmond abnehmend", 210, 435, 0);
    myFiles.load(270, 350, 60, 60, "waningcrescent.raw", 1 , 0);
     // Serial.println(phase);
     }
  }
  
  void rendermoondays() { 
  
    DateTime moon2 = rtc.now();
  
   double jdnfm = 0; // Julian Date
  double ednfm = 0; //days elapsed since start of full moon
  int b= 0;
  jdnfm = julianDate(moon2.year(), moon2.month(), moon2.day());
  //jd = julianDate(1972,1,1); // used to debug this is a new moon
  jdnfm = int(jdnfm - 2244116.75); // start at Jan 1 1972
  jdnfm /= 29.53; // divide by the moon cycle    
  b = jdnfm;
  jdnfm -= b; // leaves the fractional part of jd
  ednfm = jdnfm * 29.53; // days elapsed this month
  nfm = String((int(29.53 - ednfm))); // days to next full moon
  //Serial.println(nfm);
  char* newNfm = (char*) nfm.c_str();   
  myGLCD.setColor(255, 255, 255);
  myGLCD.print((newNfm), 340, 350, 0); 
  
  }
  
   void rendersonne() { 
  
  //Sonne
 
 DateTime sun = rtc.now();
     
 //   snprintf_P(sunbuf,sizeof(sunbuf),timeFormat,sun.day(),sun.month(),sun.year(),sun.hour(),sun.minute(),sun.second());
 //   Serial.println(sunbuf);   // show it on serial
 
     // now do the calculation for dawn/sunrise/sunset/dusk times today
     int sunrise, sunset, dawn, dusk;
   
    sunCalc(sun.year(),sun.month(),sun.day(),2, LAT, LON, sunrise, sunset, dawn, dusk);
    

   //  snprintf_P(sunbuf, sizeof(sunbuf), PSTR("Dawn: %02d:%02d  Sunrise: %02d:%02d  Sunset: %02d:%02d  Dusk: %02d:%02d"), dawn/60, dawn%60, sunrise/60, sunrise%60, sunset/60, sunset%60, dusk/60, dusk%60);
   //   Serial.println(sunbuf);

 snprintf_P(sunbuf,sizeof(sunbuf),PSTR("%02d:%02d"),sunrise/60,sunrise%60);
   
    //Serial.println(sunbuf);
      myGLCD.print((sunbuf), 42, 310, 0);
      myFiles.load(0, 330, 178, 40, "Aufgang40_2.raw", 1 , 0);

 snprintf_P(sunbuf,sizeof(sunbuf),PSTR("%02d:%02d"),sunset/60,sunset%60);
   
   // Serial.println(sunbuf);
    myGLCD.print((sunbuf), 42, 400, 0);
    myFiles.load(0, 420, 178, 40, "Untergang40_2.raw", 1 , 0);
 
 }
 
  void renderheizung() { 
 
 //Heizung
  myGLCD.setFont(BVS_9); 
  myGLCD.print(dtostrf(atemp1, 5, 1, str), 540, 380, 0);
  myGLCD.print(dtostrf(atemp2, 5, 1, str), 540, 395, 0);
  myGLCD.print(dtostrf(atemp3, 5, 1, str), 540, 410, 0);
  myGLCD.print(dtostrf(atemp4, 5, 1, str), 540, 425, 0);
  myGLCD.print(dtostrf(atemp5, 5, 1, str), 540, 440, 0);
  myGLCD.print(dtostrf(atemp6, 5, 1, str), 540, 455, 0);
  
  }


//Formel Tage bis Vollmond

double julianDate(int y, int m, int d){
// convert a date to a Julian Date}
  int mm,yy;
  double k1, k2, k3;
  double j;
  
  yy = y- int((12-m)/10);
  mm = m+9;
  if(mm >= 12) {
    mm = mm-12;
  }
  k1 = 365.25 *(yy +4172);
  k2 = int((30.6001 * mm) + 0.5);
  k3 = int((((yy/100) + 4) * 0.75) -38);
  j = k1 +k2 + d + 59;
  j = j-k3; // j is the Julian date at 12h UT (Universal Time)

  return j;
}



//Formel Mondphase
  


 double MyNormalize(double v){
  
    v = v - floor(v);
    if (v < 0)
    v = v + 1;
    return v;
  }
 
  
//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

// fast integer version with rounding
//int Celcius2Fahrenheit(int celcius)
//{
//  return (celsius * 18 + 5)/10 + 32;
//}

//Fahrenheit to Celsius conversion
double Celsius(double Fahrenheit)
{
  return (Fahrenheit - 32) * 0.55555;
}


//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

        // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

        // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}

//function to calculete Humidex

float calculate_humidex(float temperature,float humidity) {
  float e;

  e = (6.112 * pow(10,(7.5 * temperature/(237.7 + temperature))) * humidity/100); //vapor pressure

  float humidex = temperature + 0.55555555 * (e - 10.0); //humidex
  return humidex;

}

//******************* BARGRAPH ************************

void drawBarBaro(int index, int value, int valueOffset){
  int maxValue = 100*0.70; //100; height factor
  
  // lower left corner coordinates for the bargrah
  int yaxis = 325; //Höhe
  int xaxis = 425; //Links/Rechts
  
  int margin = 2;
  int width = 6;
  
   // given value range (after offset) is between 0 and 100 (950-1050)
  // and display height = 100, adjust to full display range
  
  int height = (value - valueOffset)*0.70; //Set height with * factor
    if (height < 1){
  height = 0;
  }
  int x1 = index * width;
  int x2 = x1 + width - margin;
 
  // draw it
  myGLCD.setColor(10,50,10);
  myGLCD.fillRect(xaxis + x1, yaxis, xaxis + x2, yaxis-height);
  
  // top very visible
  myGLCD.setColor(50,220,50);
  int markerHeight = 5;
  if (height < markerHeight){
    markerHeight = height;
  }
  myGLCD.fillRect(xaxis + x1, yaxis-height, xaxis + x2, yaxis-height+markerHeight);
  
  // clear top (to replace previous bar if it was higher)
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(xaxis + x1, yaxis - maxValue, xaxis + x2, yaxis-height);

}

int historyBaro[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void addHistoryValueBaro(int value){

  // shift left
  for (int i = 1; i<24; i++){
    historyBaro[i-1] = historyBaro[i];
  }
  historyBaro[23] = value;
  
  drawMillibarGraph(historyBaro, 24);
}


void drawMillibarGraph(int values[], int nr_of_values){
  
  for (int i = 0; i< nr_of_values; i++){
    drawBarBaro(i,values[i], 950); // value offset 950 gives us 0 in graph for 950mBar
  }
}

