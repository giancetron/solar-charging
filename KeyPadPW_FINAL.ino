 #include <Keypad.h>  
// #include<LiquidCrystal.h>  
 #include<EEPROM.h>  
 #define buzzer A2 
 #define led2 A3
// #define led3 A4
 #define led4 2
 #define Solenoid 13 

//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x27,16,2);
//LiquidCrystal_I2C lcd(0x38,16,2); 
 
// LiquidCrystal lcd(7,6,5,4,3,2);  
 char password[4];  
 char pass[4],pass1[4];  
 int i=0;  
 char customKey=0;  
 const byte ROWS = 4; //four rows  
 const byte COLS = 3; //four columns  
 char hexaKeys[ROWS][COLS] = {  
  {'1','2','3'},  
  {'4','5','6'},  
  {'7','8','9'},  
  {'*','0','#'}  
 };  
 byte rowPins[ROWS] = {A0, A1, 8, 9}; //connect to the row pinouts of the keypad  
 byte colPins[COLS] = {10, 11, 12};   //connect to the column pinouts of the keypad  
 //initialize an instance of class NewKeypad  
 Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);   

//..................................................................................
 void setup() {  
  Serial.begin(9600);
  pinMode(Solenoid, OUTPUT);  
  pinMode(led2, OUTPUT);
//  pinMode(led3, OUTPUT); 
//  pinMode(led4, OUTPUT); 
  pinMode(buzzer, OUTPUT);  
  pinMode(1, OUTPUT);  
  //  pinMode(m12, OUTPUT); 
  delay(1000);  
   
  for(int j=0;j<4;j++)  
  EEPROM.write(j, j+49);  
  for(int j=0;j<4;j++)  
  pass[j]=EEPROM.read(j);  
 }  

//.................................................................................. 
 void loop() {  
  
  customKey = customKeypad.getKey(); 
   
  if(customKey=='#')  
  change();  
  
  if (customKey) {  
    password[i++]=customKey;   
  Serial.println(customKey);
    beep();  
  }
    
  if(i==4){  
   delay(200);  
   for(int j=0;j<4;j++)  
   pass[j]=EEPROM.read(j);  
   if(!(strncmp(password, pass,4))){  
     digitalWrite(1, HIGH);  
    digitalWrite(Solenoid, HIGH);  
    beep();   
    delay(10000);  
    i=0;  
    digitalWrite(Solenoid, LOW);  
    digitalWrite(1, LOW);  
   }  
   else  
   {  
    digitalWrite(buzzer, HIGH);    
    delay(2000);  
    i=0;  
    digitalWrite(buzzer, LOW);  
   }  
  }  
 }  

//.................................................................................. 
 void change() { 
  beep2();
  beep2();
  delay(500);
   
  int j=0;  
  digitalWrite(led2, HIGH);
  while(j<4){  
   char key=customKeypad.getKey();  
   if(key){  
    pass1[j++]=key;    
    beep();  
   }  
   key=0;  
  }  
  delay(500);  
  if((strncmp(pass1, pass, 4))){  
   digitalWrite(buzzer, HIGH); 
   delay(2000);  
   digitalWrite(led2, LOW);  
   digitalWrite(buzzer, LOW);
  }  
  else  
  {  
   j=0;  
  //lcd.clear();  
  //lcd.print("Enter New Passk:");  
  //lcd.setCursor(0,1);  
  while(j<4) {  
   char key=customKeypad.getKey();  
   if(key)  
   {  
    pass[j]=key;  
    EEPROM.write(j,key);  
    j++;  
    beep(); 
    digitalWrite(led2, LOW); 
//    digitalWrite(led3, HIGH); 
   }  
  }  
  //lcd.print(" Done......"); 
  beep2();
  delay(300);
  beep2();
  delay(300);
  beep2();
  delay(300);
  beep2();
//  digitalWrite(led3, LOW); 
  digitalWrite(buzzer, HIGH);
  delay(500); 
  digitalWrite(buzzer, LOW); 
  }   
  customKey=0;  
 }  

//.................................................................................. 
 void beep()  {   
  digitalWrite(buzzer, HIGH);  
  delay(20);  
  digitalWrite(buzzer, LOW);  
 }  
//.................................................................................. 
 void beep2()  {   
  digitalWrite(buzzer, HIGH);  
  delay(250);  
  digitalWrite(buzzer, LOW);  
 } 
 
