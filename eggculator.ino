/*
Eier-Rechner

http://newton.ex.ac.uk/teaching/CDHW/egg/
http://www.eierfans.de/eirechner-wie-lange-eier-kochen/

PB, 20170813
*/
#include "Wire.h" // For I2C
#include "LCD.h"  // For LCD
#include "LiquidCrystal_I2C.h"
#include <Bounce2.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SET 5
#define INCREASE 7
#define DECREASE 6
#define TEMP_BUS 8

// Set the pins on the I2C chip used for LCD connections
// ADDR,EN,R/W,RS,D4,D5,D6,D7
// Read the following article to find the address
// http://www.electroschematics.com/12459/arduino-i2c-lcd-backpack-introductory-tutorial/
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7);

// Prepare buttons
boolean SET_button = false, L_SET = false;
Bounce db_SET_button = Bounce();
boolean DECREASE_button = false, L_DECREASE = false;
Bounce db_DECREASE_button = Bounce();
boolean INCREASE_button = false, L_INCREASE = false;
Bounce db_INCREASE_button = Bounce();

// Variables
int mode = 0;
float m = 70;  // Mass of the egg in [g]
float T_water; // Water temperature [deg C]
float T_start; // User inputs
float L;       // Avg. heat capacity of an egg
float t;       // Required time in [minutes]
float t_sec;   // Fraction of t in [s]

float T_inside[3] = {62, 67, 82}; // Temperature required to achieve a certain texture
char* texture[3] = {"Fluessig        ", "Weich           ", "Hart           "}; // Texture strings
int texture_idx = 0; // Index of selected texture

unsigned long msecs; // Timestamp in [ms]

// Prepare temperature sensor
OneWire onewireTemp(TEMP_BUS);
DallasTemperature temp_sensor(&onewireTemp);


void setup()
{
  // Serial.begin(115200);
  
  // Avg. heat capacity of an egg
  L = (0.517 * 0.33) + (0.450 * 0.67);
  
  // Setup LCD module
  lcd.begin (16, 2); // 16 x 2 LCD module
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  
  // Setup buttons
  pinMode(SET, INPUT_PULLUP);
  digitalWrite(SET, HIGH);
  db_SET_button.attach(SET);
  db_SET_button.interval(5);
  pinMode(DECREASE, INPUT_PULLUP);
  digitalWrite(DECREASE, HIGH);
  db_DECREASE_button.attach(DECREASE);
  db_DECREASE_button.interval(5);
  pinMode(INCREASE, INPUT_PULLUP);
  digitalWrite(INCREASE, HIGH);
  db_INCREASE_button.attach(INCREASE);
  db_INCREASE_button.interval(5);
  
  //Init temperature sensor
  temp_sensor.begin();
  // Reading a new value takes about 750 ms. To prevent this from blocking the user interface, don't wait for it.
  // Instead manually check whether 750 ms elapsed and update the display if true.
  temp_sensor.setWaitForConversion(false);
  msecs = millis();
}
void loop()
{
  // Poll buttons
  db_SET_button.update();
  SET_button = db_SET_button.rose();
  if(SET_button == true){
    L_SET = true;
    mode++;
    if(mode == 1){
      lcd.clear();
      lcd.home();
      lcd.print("Start-Ei-Temp.  ");
    }
    if(mode == 2){
      lcd.clear();
      lcd.home();
      lcd.print("Gewicht [g]     ");
    }
    if(mode == 3){
      lcd.clear();
      lcd.home();
      lcd.print("Konsistenz      ");
    }
    if(mode == 4){
      // mode 4 obsolete because the temperature is read by DS18b20-Sensor
      mode++;
      //lcd.clear();
      //lcd.home();
      //lcd.print("Wasser-Temp.    ");
    }
    if(mode == 5){
      lcd.clear();
      lcd.home();
      //lcd.print("Zeit [Min:Sec]  ");
    }
    if(mode == 6){
      mode = 0;
      lcd.clear();
      L_SET = false;
    }
  }
  db_DECREASE_button.update();
  DECREASE_button = db_DECREASE_button.rose();
  if(DECREASE_button == true){
    L_DECREASE = true;
  }
  db_INCREASE_button.update();
  INCREASE_button = db_INCREASE_button.rose();
  if(INCREASE_button == true){
    L_INCREASE = true;
  }

  // Handle individual modes
  // Start screen
  if(mode == 0){
    lcd.home();
    lcd.print("Eier-Rechner    ");
  }

  // Input start temperature of the egg
  if(mode == 1){
    if(L_DECREASE){
      L_DECREASE = false;
      T_start--;
    }
    if(L_INCREASE){
      L_INCREASE = false;
      T_start++;
    }
    lcd.setCursor (0,1);
    lcd.print(T_start);
  }

  // Input mass of the egg
  if(mode == 2){
    if(L_DECREASE){
      L_DECREASE = false;
      m--;
    }
    if(L_INCREASE){
      L_INCREASE = false;
      m++;
    }
    lcd.setCursor (0,1);
    lcd.print(m);
  }

  // Select desired texture
  if(mode == 3){
    if(L_DECREASE){
      L_DECREASE = false;
      if(texture_idx == 0){
        texture_idx = 2;
      }
      else{
        texture_idx--;
      }
    }
    if(L_INCREASE){
      L_INCREASE = false;
      if(texture_idx == 2){
        texture_idx = 0;
      }
      else{
        texture_idx++;
      }
    }
    lcd.setCursor (0,1);
    lcd.print(texture[texture_idx]);
  }

  // Ignored
  if(mode == 4){
    // Obsolete, see above
    if(L_DECREASE){
      L_DECREASE = false;
      T_water--;
    }
    if(L_INCREASE){
      L_INCREASE = false;
      T_water++;
    }
    if(T_water <= T_inside[texture_idx]){
      // Formula invalid if T_water is less or equal than T_inside
      T_water = T_inside[texture_idx]+1;
    }
    lcd.setCursor (0,1);
    lcd.print(T_water);
  }

  // Calculate and display result, update when 750 ms are over
  if(mode == 5){
    if(millis() - msecs > 750)
    {
      msecs = millis();
      temp_sensor.requestTemperatures();
      T_water = temp_sensor.getTempCByIndex(0);
      //Serial.println(T_water);
      t = L * pow(m, 0.67) * log(0.76 * (T_start - T_water) / (T_inside[texture_idx] - T_water));
      t_sec = (t - int(t)) * 60;
      lcd.home();
      lcd.print(T_water);
      lcd.setCursor(6,0);
      lcd.print((char)223); // Degree symbol
      lcd.print("C         ");
      lcd.setCursor(0,1);
      // Catch case when t == nan; Happens if the water is too cold, i.e. below the temperatures listed in T_inside
      if(isnan(t)){
        lcd.print("Zu kalt!        ");
      }
      else{
        if(int(t) < 10){lcd.print("0");}
        lcd.print(int(t));
        lcd.print(":");
        if(int(t_sec) < 10){lcd.print("0");}
        lcd.print(int(t_sec));
      }
    }
  }
}

