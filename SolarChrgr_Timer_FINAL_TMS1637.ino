#include <TM1637Display.h>
#define CLK 4
#define DIO 5
TM1637Display display(CLK, DIO); //set up the 4-Digit Display.

#define coinSlotPin 7
#define chargingPort 10

const uint8_t blank[] = {0x00, 0x00, 0x00,0x00};

boolean insertedCoin = false;
int totalCoin = 0;
int coinSlotStatus;
int chargeTime = 2;     //per inute //1peso = 5min

unsigned long previousMillis = 0;
const long interval = 1000;  //1000

int hourUnit = 0;
int minUnit = 0;
int secUnit = 0;

int minOffSetValue = 0;
boolean countdown = false;


void setup() {
  Serial.begin(9600);
  pinMode(coinSlotPin, INPUT_PULLUP);
  pinMode(chargingPort, OUTPUT);
  digitalWrite(chargingPort, LOW);
    //display.setBrightness(0x0a); //set the diplay to maximum brightness
    display.setBrightness(7); // Set the brightness of the display (0-7)
    display.setSegments(blank);//clear display
}

void loop() {
  while(!insertedCoin) {
    coinSlotStatus = digitalRead(coinSlotPin);
    timer();
    if(coinSlotStatus == 0) {
      countdown = true;
      insertedCoin = true;
      totalCoin += 1;
      minUnit =  (totalCoin * chargeTime) + minOffSetValue - 1;
      secUnit = 59;

        if(minUnit >= 60) {
          hourUnit += 1;
          minUnit = minUnit - 60;

          if (minUnit > 0) {
            minOffSetValue = minUnit;
          }
          totalCoin = 0;
        }
      }
    }
  while(insertedCoin) {
    coinSlotStatus = digitalRead(coinSlotPin);
    if (coinSlotStatus == 1) {
      insertedCoin = false;
    }
  }
}


void timer() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval){;
    previousMillis = currentMillis;
    printTime();
    countDown();
  }
}


void printTime() {
  if(countDown) {

    
    // Display remaining time in Minutes:Seconds format
    unsigned int minutes = minUnit ;/// 60;
    unsigned int seconds = secUnit % 60; 
    display.showNumberDec(minutes * 100 + seconds, true, 4, 0); 

//    if(hourUnit < 10) {
//      Serial.print("0");Serial.print(hourUnit);
//    } else {
//      Serial.print(hourUnit);
//    }
//    Serial.print(":");
//    if(minUnit < 10) {
//      Serial.print("0");Serial.print(minUnit);
//    } else {
//      Serial.print(minUnit);
//    }
//    Serial.print(":");
//    if(secUnit < 10) {
//      Serial.print("0");Serial.print(secUnit);
//    } else {
//      Serial.println(secUnit);
//    }   
 
  }
     digitalWrite(chargingPort, HIGH);  
}

void countDown () {
  if(secUnit > 0) {
    secUnit -= 1;
      if(minUnit > 0){
          if(secUnit < 1) {
          secUnit = 0;
          printTime();
          delay(interval);
          minUnit -= 1;
          secUnit = 59;
        }
      }
    } else {
    totalCoin = 0;
    countdown = false;
    digitalWrite(chargingPort, LOW);
    Serial.println("Insert Coin to Start Charging"); 
    }
  }
