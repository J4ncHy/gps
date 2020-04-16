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
|  LED+  | D5 + 220Ω |
|   SD_CS | D4   |
|   MOSI  | D11   |
|   MISO  | D12   |
|   SCK   | D13   |
|   CS    | D10 + 2k7 Ω|
|   SCK   | D13 + 2k7 Ω |
|   SDA   | D11 + 2k7 Ω |
|   A0    | D8  + 2k7 Ω |
|   RESET | D9  + 2k7 Ω |
|   null |   |
|   null |   |
|   null |   |
|   VCC   | +5 V   |
|   GND   | GND   |

GPS:
| Pin name  | Connection |
| --------- | ------------- |
| TX| D2 |
| RX | D3 |
| GND  | GND |
| VCC | +5 V |


Takes data from a NEO-6M GPS module and writes then onto an SD card in the folder dd-mm-yy and into a hh-mm.gpx file.
It also displays some info on the 1.8 SPI TFT display.

To see your recorded path, go to https://www.google.com/maps/d/ and make an new map. Then import the .gpx file into a new layer.
