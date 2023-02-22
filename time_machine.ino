#include <Wire.h> 
#include <hd44780.h>                      
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define PIN 10
#define NUMPIXELS 8

hd44780_I2Cexp lcd;
hd44780_I2Cexp lcd2(0x27);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint16_t record = 9999;
long start = 0;
long lastTime = 0;
long reaction = 0;
const int trig = 3;
const int reset = 4;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
int buttonState = 0;
int button2State = 0;
long lastMillis = 0;
int rectime = 3000;
int state = 0;
int t = 0;
int col_sat = 50;
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
  pixels.begin();
  pixels.clear();
  pixels.show();
  delay(1000);

  button2State = digitalRead(reset);

  //sprawdzenie zawartości EEPROM
  record_hi = EEPROM.read(0);
  record_lo = EEPROM.read(1);

  now_record = (record_hi << 8) + record_lo;

  delay(500);

  //Wyświetlenie informacji o nie prawidłowej lub resecie wartości EEPROM.
  if (now_record > 9999 || now_record == 0 || button2State == HIGH){
    if (button2State == HIGH){
      for (int i=0; i<=10; i++){
        pixels.setPixelColor((0), pixels.Color(250, 250, 250));
        pixels.show();
        delay(150);
        pixels.clear();
        pixels.show();
        delay(150);
      }
    }

    //Wpisanie wartośći początkowej do EEPROM (9999).
    record_hi = record >> 8;
    record_lo = record & 0x00FF;
    //Zapis do EEPROM.
    EEPROM.write(0, record_hi);
    EEPROM.write(1, record_lo);
    pixels.setPixelColor((0), pixels.Color(250, 250, 250));
    pixels.show();
    delay(1000);
    pixels.clear();
    pixels.show();
  }

  //Ponowny odczyt rekordu z pamięci
  record_hi = EEPROM.read(0);
  record_lo = EEPROM.read(1);

  //Wpisanie liczby z EEPROM do zmiennej RECORD.
  record = (record_hi << 8) + record_lo;

  //inicjalizacja przerwania.
  cli();          
  TCCR1A = 0;     
  TCCR1B = 0;     
  TCCR1B |= B00000101;
  TIMSK1 |= B00000010;
  OCR1A = 31250;
  sei();


  pinMode(trig, INPUT);
  lcd.begin(LCD_COLS, LCD_ROWS);              
  lcd.createChar(0, Block);
  lcd.createChar(1, Blank);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("MADE BY:");
  lcd.setCursor(3,1);
  lcd.print("@PJ_MAKER");
  delay(1000);

  //Animacja na led.
  for(int i=0; i<=16; i++){
    lcd.setCursor(i,0);
    lcd.write(0);
    lcd.setCursor(i,1);
    lcd.write(0);
    pixels.setPixelColor((i), pixels.Color(col_sat, 0, 0));
    pixels.show();
    delay(50);
  }

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
}

void Times(){
  if(state > 1) state = 0;
  if(state == 0){
    lcd.print("RECORD: ");
    lcd.print(record);
    lcd.print("ms");
    for (int j=0; j<4; j++)
    {
      lcd.write(1);
    }
  }
  else{  
    lcd.print("LAST:   ");
    lcd.print(lastTime);
    lcd.print("ms");
    for (int k=0; k<4; k++)
    {
      lcd.write(1);
    }
  }
}

void clearLine(int l){
  for(int i=0; i<16; i++){
    lcd.setCursor(i,l);
    lcd.write(1);
    lcd.setCursor(0,1);
  }
}

void loop() {
  lcd.backlight();
  
  for(int m=0; m<=15; m++){
    pixels.setPixelColor(m, pixels.Color(col_sat, 0, 0));
    pixels.show();
  }

  buttonState = digitalRead(trig);
  Ready(); 
  lcd.setCursor(0,1);
  Times();
  t=0;

  while (buttonState == HIGH){
    for(t; t<1; t++){
      lcd.clear();
      lcd.noBacklight();
      lastTime++;
      start = random(500, 3500);
    }

    for(int n=0; n<=8; n++){
      pixels.setPixelColor(n, pixels.Color(col_sat, 0, 0));
      pixels.show();
    }
    
    delay(1500);
   
    for(int o=0; o<=8; o++){
      pixels.setPixelColor(o, pixels.Color(0, col_sat, 0));
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
      record_hi = record >> 8;
      record_lo = record & 0x00FF;
      EEPROM.write(0, record_hi);
      EEPROM.write(1, record_lo);
      lcd.setCursor(0,0);
      lcd.print("NEW RECORD TIME:");
      for(int h=0; h<=6; h++){
        for(int j=0; j<=8; j++){
          pixels.setPixelColor(j, pixels.Color(col_sat, col_sat, 0));
          pixels.show();
        }
        delay(500);
        pixels.clear();
        pixels.show();
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
