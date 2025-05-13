#include <Arduino.h>
#include <EEPROM.h>
#include <elapsedMillis.h>
#include <WiFi.h>
#include <WebServer.h>
//#include "soc/soc.h"
//#include "soc/rtc_cntl_reg.h"

float b_percent;
float b,t, tF;
float ADC_VALUE = 0;
float vout = 0.0;
float vin = 0.0;
float R1 = 100000;
float R2 = 15000; //10000
int value = 0;


// Sum of all the  coins inserted
int totalAmount1 = 0;
int totalAmount2 = 0;
int totalAmount3 = 0;

//volatile bool updateDisplay = false;
volatile bool coinInserted1 = false;
volatile bool coinInserted2 = false;
volatile bool coinInserted3 = false;


elapsedMillis timer;
elapsedMillis countDownTimer;
long interval_timer = 10000;
bool countDownStarted = false;

const uint8_t PIN1 = 33;  
const uint8_t PIN2 = 25;
const uint8_t PIN3 = 26;
const int RstButton = 34;  // this will assign the name PushButton to pin numer 34 
const uint8_t RELAY_PIN = 13;

void turnOnRelay();
void turnOffRelay();

void IRAM_ATTR PIN1_ISR() { coinInserted1 = true;}
void IRAM_ATTR PIN2_ISR() { coinInserted2 = true;}
void IRAM_ATTR PIN3_ISR() { coinInserted3 = true;}



/*Put your SSID & Password*/
//const char* ssid = "TP-Link_55A2"; //Wifi Name SSID
//const char* password = "46458186"; //WIFI Password
//-----------------------   AP MODE ---------------------------------;
const char* ssid = "SOLAR_CHRGR_WEBSERVER";
const char* password = "12345678";
/*------------------------------------------------------------------*/
/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80); 
       
//int thermoDO = 19;    // SO of  MAX6675 module to D19
//int thermoCS = 5;     // CS of MAX6675 module to D5
//int thermoCLK = 18;   // SCK of MAX6675 module to D18
//MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);



void setup() {
  Serial.begin(9600);
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  pinMode(PIN1, INPUT);
  pinMode(PIN2, INPUT);
  pinMode(PIN3, INPUT);
  pinMode(RstButton, INPUT);

  attachInterrupt(PIN1, PIN1_ISR, FALLING);
  attachInterrupt(PIN2, PIN2_ISR, FALLING);
  attachInterrupt(PIN3, PIN3_ISR, FALLING);

//-----------------      ESP32 As access point  ---------------------------;
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
///*--------------------------------------------------------------------------*/
 
//  Serial.println("Connecting to ");
//  Serial.println(ssid);
//  WiFi.begin(ssid, password);  //connect to your local wi-fi network
//  while (WiFi.status() != WL_CONNECTED) {  //check wi-fi is connected to wi-fi network
//  delay(500);
//  Serial.print(".");
//  }
  
  Serial.println("");
  Serial.println("WiFi connected..!");
//  Serial.print("Got IP: ");  
//  Serial.println(WiFi.localIP());
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  
}


void loop() {
 
  server.handleClient();
  
  if (digitalRead(RstButton) == LOW)  {
    while (digitalRead(RstButton) == LOW)
    totalAmount1 = 0;
    totalAmount2 = 0;
    totalAmount3 = 0;
  }
    
  if (coinInserted1)  {
    coinInserted1 = false;
    delay(70);
    totalAmount1 += 1;
    //totalAmount1 = totalAmount1 + 1;
  }

  if (coinInserted2)  {
    coinInserted2 = false;
    delay(70);
    totalAmount2 += 1;
    //totalAmount2 = totalAmount2 + 1;
  }  

  if (coinInserted3)  {
    coinInserted3 = false;
    delay(70);
    totalAmount3 += 1;    
    //totalAmount3 = totalAmount3 + 1;
  }

  
}




void handle_OnConnect() {
  //* -----------   BATTERY   ------------ */   
  vout = (analogRead(A0) * 3.3) / 4095;
  b = vout/(R2/(R1+R2));
  b_percent =  map(analogRead(A0), 0.0f, 4095.0f, 0, 199);  
  server.send(200, "text/html", SendHTML(t,tF)); 
  Serial.println(b);
  Serial.println(b_percent);
}


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}



String SendHTML(float t,float tF){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>Solar Charger Monitor</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr +="<style>";
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
  ptr +="body{margin: 0px;} ";
  ptr +="h1 {margin: 50px auto 30px;} ";
  ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +=".text{font-weight: 600;font-size: 25px;width: 200px;}";
  ptr +=".reading{font-weight: 300;font-size: 50px;padding-right: 25px;}";
  
  ptr +=".b .reading{color: #FF1A1A;}"; 
  ptr +=".b_percent.reading{color: #15C11B;}"; 
  
  ptr +=".t .reading{color: #F29C1F;}";
  ptr +=".tF .reading{color: #3B97D3;}";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;top: 10px;}";
  ptr +=".data{padding: 10px;}";
  ptr +=".container{display: table;margin: 0 auto;}";
  ptr +=".icon{width:65px}";
  ptr +="</style>";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,1000);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.body.innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";
  ptr +="</head>";
  ptr +="<body>";
  ptr +="<h1>Solar Charger Monitor</h1>";
  ptr +="<div class='container'>";
 
//  ptr +="<div class='data t'>";
//  ptr +="<div class='side-by-side icon'>";
//  ptr +="<svg enable-background='new 0 0 19.438 54.003'height=54.003px id=Layer_1 version=1.1 viewBox='0 0 19.438 54.003'width=19.438px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M11.976,8.82v-2h4.084V6.063C16.06,2.715,13.345,0,9.996,0H9.313C5.965,0,3.252,2.715,3.252,6.063v30.982";
//  ptr +="C1.261,38.825,0,41.403,0,44.286c0,5.367,4.351,9.718,9.719,9.718c5.368,0,9.719-4.351,9.719-9.718";
//  ptr +="c0-2.943-1.312-5.574-3.378-7.355V18.436h-3.914v-2h3.914v-2.808h-4.084v-2h4.084V8.82H11.976z M15.302,44.833";
//  ptr +="c0,3.083-2.5,5.583-5.583,5.583s-5.583-2.5-5.583-5.583c0-2.279,1.368-4.236,3.326-5.104V24.257C7.462,23.01,8.472,22,9.719,22";
//  ptr +="s2.257,1.01,2.257,2.257V39.73C13.934,40.597,15.302,42.554,15.302,44.833z'fill=#F29C21 /></g></svg>";
//  ptr +="</div>";
//  ptr +="<div class='side-by-side text'>Temp in Celsius</div>";
//  ptr +="<div class='side-by-side reading'>";
//  ptr +=(int)t;
//  ptr +="<span class='superscript'>&deg;C</span></div>";
//  ptr +="</div>";
//  ptr +="<div class='data tF'>";
//  ptr +="<div class='side-by-side icon'>";
//  ptr +="<svg enable-background='new 0 0 19.438 54.003'height=54.003px id=Layer_1 version=1.1 viewBox='0 0 19.438 54.003'width=19.438px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M11.976,8.82v-2h4.084V6.063C16.06,2.715,13.345,0,9.996,0H9.313C5.965,0,3.252,2.715,3.252,6.063v30.982";
//  ptr +="C1.261,38.825,0,41.403,0,44.286c0,5.367,4.351,9.718,9.719,9.718c5.368,0,9.719-4.351,9.719-9.718";
//  ptr +="c0-2.943-1.312-5.574-3.378-7.355V18.436h-3.914v-2h3.914v-2.808h-4.084v-2h4.084V8.82H11.976z M15.302,44.833";
//  ptr +="c0,3.083-2.5,5.583-5.583,5.583s-5.583-2.5-5.583-5.583c0-2.279,1.368-4.236,3.326-5.104V24.257C7.462,23.01,8.472,22,9.719,22";
//  ptr +="s2.257,1.01,2.257,2.257V39.73C13.934,40.597,15.302,42.554,15.302,44.833z'fill=#3C97D3 /></g></svg>";
//  ptr +="</div>";
//  ptr +="<div class='side-by-side text'>Temp in Fahrenheit</div>";
//  ptr +="<div class='side-by-side reading'>";
//  ptr +=(int)tF;
//  ptr +="<span class='superscript'>&deg;F</span></div>";
//  ptr +="</div>";
  
  ptr +="<div class='data b'>";
  ptr +="<div class='side-by-side text'>Battery Voltage  </div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(float)b;
  ptr +="<span class='superscript'>Volts </span></div>";
  ptr +="</div>";

  ptr +="<div class='data b_percent'>";
  ptr +="<div class='side-by-side text'>Battery Capacity  </div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(float)b_percent;
  ptr +="<span class='superscript'>%</span></div>";
  ptr +="</div>";

  ptr +="<div class='data totalAmount1'>";
  ptr +="<div class='side-by-side text'>CELL Charger 1 Sales  </div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)totalAmount1;
  ptr +="<span class='superscript'>Pesos</span></div>";
  ptr +="</div>";  

  ptr +="<div class='data totalAmount2'>";
  ptr +="<div class='side-by-side text'>CELL Charger 2 Sales  </div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)totalAmount2;
  ptr +="<span class='superscript'>Pesos</span></div>";
  ptr +="</div>";
  
  ptr +="<div class='data totalAmount3'>";
  ptr +="<div class='side-by-side text'>PC Charger Sales  </div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)totalAmount3;
  ptr +="<span class='superscript'>Pesos</span></div>";
  ptr +="</div>";
     
  ptr +="</body>";
  ptr +="</html>";
  return ptr;
}
