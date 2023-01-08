#include <Wire.h> 
#include <LiquidCrystal_I2C.h>,
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define PIN 10
#define NUMPIXELS 16

LiquidCrystal_I2C lcd(0x27,20,4);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

long record = 9999;
long start = 0;
long lastTime = 0;
long reaction = 0;
const int trig = 3;
const int reset = 4;
const int led = 13;
int buttonState = 0;
int button2State = 0;
long lastMillis = 0;
int rectime = 3000;
int state = 0;
int t=0;
int col_sat = 250;
uint16_t now_record;
byte record_hi;
byte record_lo;

byte Block[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000
};

byte Blank[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};


void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  delay(3000);
  Serial.println("Hello, I'm DEBUG.");
  Serial.println(" ");
  button2State = digitalRead(reset);
  delay(3000);
  record_hi = EEPROM.read(0);
  record_lo = EEPROM.read(1);
  Serial.println("values in eeprom 8bit: ");
  Serial.print("hi: ");
  Serial.println(record_hi,HEX);
  Serial.print("lo: ");
  Serial.println(record_lo,HEX);

  now_record = (record_hi <<8) + record_lo;
  Serial.println("values in eeprom 16bit: ");
  Serial.print("now_record: ");
  Serial.println(now_record);
  Serial.println(" ");

  delay(3000);
  if (now_record > 9999 || now_record == 0 || button2State == HIGH){
    if (button2State == HIGH){
      Serial.println("RESETING RECORD TO DEFAULT!");
      for (int i = 0; i <= 10; i++){
        digitalWrite(led, HIGH);
        delay(150);
        digitalWrite(led, LOW);
        delay(150);
      }
    }
    Serial.print("Write new record to EEPROM. HIGH: ");
    digitalWrite(led, HIGH);
    record_hi = record >> 8;
    record_lo = record & 0x00FF;
    EEPROM.write(record_hi, 0);
    EEPROM.write(record_lo, 1);

    Serial.print(record_hi,HEX);
    Serial.print(" LOW: ");
    Serial.print(record_lo,HEX);
    Serial.println(" ");
    delay(1000);
    digitalWrite(led, LOW);
  }

  delay(3000);

  record_hi = EEPROM.read(0);
  record_lo = EEPROM.read(1);
  Serial.println("values in eeprom 8bit: ");
  Serial.print("hi: ");
  Serial.println(record_hi);
  Serial.print("lo: ");
  Serial.println(record_lo);

  now_record = (record_hi <<8) + record_lo;
  Serial.println("values in eeprom 16bit: ");
  Serial.print("now_record: ");
  Serial.println(now_record);
  Serial.println(" ");

  delay(10000);

  Serial.println("end");


  cli();          
  TCCR1A = 0;     
  TCCR1B = 0;     
  TCCR1B |= B00000101;
  TIMSK1 |= B00000010;
  OCR1A = 31250;
  sei();

  pinMode(trig, INPUT);
  
  lcd.init();                      
  lcd.init();
  lcd.createChar(0, Block);
  lcd.createChar(1, Blank);
  lcd.backlight();
  
  pixels.begin();
  pixels.clear();
  pixels.show();

  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("MADE BY:");
  lcd.setCursor(3,1);
  lcd.print("@PJ_MAKER");
  delay(1000);
  
  for(int i=0;i<=16;i++){
    lcd.setCursor(i,0);
    lcd.write(0);
    lcd.setCursor(i,1);
    lcd.write(0);
    pixels.setPixelColor((i), pixels.Color(col_sat, 0, 0));
    pixels.show();
    delay(50);
  }
  Serial.println("LAUNCHING...");
  delay(3000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("REACTION TIME");
  lcd.setCursor(5,1);
  lcd.print("TESTER");
  delay(4000);
  lcd.clear();
}


void Ready(){
  lcd.setCursor(5,0);
  lcd.print("READY?");
  Serial.println("Ready to  ROCK!!");
}

void Times(){
  if(state > 1) state =0;
  if(state == 0){
    lcd.print("RECORD: ");
    lcd.print(record);
    lcd.print("ms");
    lcd.write(1);
    lcd.write(1);
    lcd.write(1);
    lcd.write(1);
    lcd.write(1);
  }
  else{  
    lcd.print("LAST:   ");
    lcd.print(lastTime);
    lcd.print("ms");
    lcd.write(1);
    lcd.write(1);
    lcd.write(1);
    lcd.write(1);
    lcd.write(1);
    
  }
  
}

void clearLine(int l){
  for(int i=0;i<16;i++){
    lcd.setCursor(i,l);
    lcd.write(1);
    lcd.setCursor(0,1);
  }
}

void loop() {
  lcd.backlight(); 
  
  for(int j=0; j<=15;j++){
    pixels.setPixelColor(j, pixels.Color(col_sat, 0, 0));
    pixels.show();
  }

  buttonState = digitalRead(trig);
  Ready(); 
  lcd.setCursor(0,1);
  Times();
  t=0;

  while (buttonState == HIGH){
    Serial.println("Waiting for clutch relase");
    for(t;t<1;t++){
      lcd.clear();
      lcd.noBacklight();
      lastTime++;
      start = random(1000, 4000);
      Serial.println(start);
    }

    for(int j=0; j<=15;j++){
      pixels.setPixelColor(j, pixels.Color(col_sat, 0, 0));
      pixels.show();
    }
    
    delay(1500);
   
    for(int j=0; j<=15;j++){
      pixels.setPixelColor(j, pixels.Color(0, col_sat, 0));
      pixels.show();
    }
    
    delay(start);
  
    buttonState = digitalRead(trig);
    
    if(buttonState == LOW){
      lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print("F A L S T A R T");
      delay(2000);
      lcd.clear();
      break;
    }
    
    pixels.clear();
    pixels.show();    
    lastMillis = millis();
    
    while(buttonState == HIGH){
      buttonState = digitalRead(trig);  
    }
    
    reaction = millis() - lastMillis;
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("YOUR TIME:");
    lcd.setCursor(0,1);
    lcd.print(reaction);
    lcd.print("ms");
    lastTime = reaction;
    if(reaction < record){
      record = reaction;
      lcd.setCursor(0,0);
      lcd.print("NEW RECORD TIME:");
      for(int h=0;h<=6;h++){
        for(int j=0; j<=15;j++){
          pixels.setPixelColor(j, pixels.Color(col_sat, col_sat, 0));
          pixels.show();
        }
        delay(500);
        for(int j=0; j<=15;j++){
          pixels.setPixelColor(j, pixels.Color(0, 0, 0));
          pixels.show();
        }
        delay(500);
       }       
     }
     delay(3000);
     lcd.clear();
     break;
   }
}


   

ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;
  state++;  
}
