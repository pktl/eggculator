# eggculator
An Arduino-based device which calculates the time required to cook an egg.

# Hardware
Required components:
* Arduino
* 16x2 LCD with I2C interface board
* 3 buttons
* DS18B20 Temperature sensor
* 4.7k Resistor
eggculator_schematic.fzz contains schematics on how to wire up all components.

# Software
Required libraries:
* LiquidCrystal_I2C (http://www.electroschematics.com/wp-content/uploads/2015/12/Liquid-Crystal-Libray-Download.rar)
* Bounce2           (https://github.com/thomasfredericks/Bounce2)
* OneWire           (https://github.com/PaulStoffregen/OneWire)
* DallasTemperature (https://github.com/milesburton/Arduino-Temperature-Control-Library)

I recommend to read http://www.electroschematics.com/12459/arduino-i2c-lcd-backpack-introductory-tutorial/ to correctly set up the LiquidCrystal_I2C library.

# Usage
1. Start boiling water.
2. Power up the Eggculator and put the temperature sensor in the water.
3. Weigh your egg and measure it's temperature (or estimate. Usually around 8 degree C out of the fridge).
4. Feed your Eggculator with the appropriate numbers and select a texture you like.
5. Setup your kitchen timer with the time calculated by the Eggculator.
6. Enjoy! :)
