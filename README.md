# GPS
An arduino project for my matura project.

The program is using the following libraries:
Adafruit GFX library - can use the preloaded TFT libarary instead of this,
Adafruit ST7735 and ST7789 Library - can use the preloaded TFT libarary instead of this,
SoftwareSerial preloaded library,
SPI and SD libraries and
https://github.com/mikalhart/TinyGPSPlus.

CONNECTIONS:
Display:


| Pin name  | Connection |
| --------- | ------------- |
| LED-  | GND  |
|  LED+  | D5  |
|   SD_CS | D4   |
|   MOSI  | D11   |
|   MISO  | D12   |
|   SCK   | D13   |
|   CS    | D10   |
|   SCK   | D13   |
|   SDA   | D11   |
|   A0    | D8   |
|   RESET | D9   |
|   null |   |
|   null |   |
|   null |   |
|   VCC   | +5 V   |
|   GND   | GND   |


Takes data from a NEO-6M GPS module and writes then onto an SD card in the folder dd-mm-yy and into a hh-mm.gpx file.
It also displays some info on the 1.8 SPI TFT display.

To see your recorded path, go to https://www.google.com/maps/d/ and make an new map. Then import the .gpx file into a new layer.
