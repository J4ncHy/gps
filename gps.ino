#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define chipSelect 4
static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;

static void setData();
void volti();
void writeInfo();
static void smartDelay(unsigned long ms);
static void printInt(unsigned long val, bool valid, int len);

// The TinyGPS++ object
TinyGPSPlus gps;

char filename1[20];
char date1[22];
char filepath[20];
unsigned long start;
float latitude, longitude, speed;
int year, h_dop;
byte month, day, hour, minute, second;
byte loopA;
File dataFile;
#define PIN_SD_CS 4
#define TFT_CS     10
#define TFT_RST    9  
#define TFT_DC     8
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  pinMode(6,OUTPUT);
  digitalWrite(6, HIGH);
  tft.initR(INITR_BLACKTAB); //Init
  tft.fillScreen(ST7735_BLACK); //Fill with black
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK); //White text, black background
  tft.setRotation(1);
  delay(1000);

  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.println("Satellites: ");
  
  while (true)
  {
    if (!SD.begin(chipSelect))
    {
      Serial.println(F("Card failed"));
      volti();
    }
    else
    {
      Serial.println("SD card ready");
      break;
    }
  }

  while (true)
  {
    setData();
    unsigned long start = millis();
    Serial.println(gps.satellites.value());
    if (gps.satellites.isValid())
    {
      hour = hour + 1;
      if (hour > 23)
      {
        hour = hour - 24;
        day += 1;
      }
      sprintf(filename1, "/%02d-%02d-%02d", day, month, year - 2000);
      sprintf(filepath, "/%02d-%02d-%02d/%02d-%02d%s", day, month, year - 2000, hour, minute, ".GPX");
      Serial.println(filepath);
      SD.mkdir(filename1);
      tft.setCursor(0,16);
      tft.println("Start time: ");
      tft.setCursor(50,16);
      tft.println(hour);
      tft.setCursor(60,16);
      tft.println(minute);
      
      if (!SD.exists(filepath))
      {
        dataFile = SD.open(filepath, FILE_WRITE);
        dataFile.print(F(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
            "<gpx version=\"1.1\" creator=\"Batuev\" xmlns=\"http://www.topografix.com/GPX/1/1\" \r\n"
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n"
            "xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\r\n"
            "\t<trk>\r\n<trkseg>\r\n")); //heading of gpx file
        dataFile.print(F("</trkseg>\r\n</trk>\r\n</gpx>\r\n"));
        dataFile.close();
      }
      break;
    }
    else {
      tft.setCursor(0,8);
      tft.println("SD card valid");
      tft.setCursor(90,0);
      tft.println(gps.satellites.value());
      Serial.println(gps.satellites.value());
    }
  }
}
void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read())){
      writeInfo();
      tft.println(gps.satellites.value());
      Serial.println(gps.satellites.value());
    }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("GPS found"));
    while (true)
      ;
  }
}

static void setData()
{
  year = gps.date.year();
  month = gps.date.month();
  day = gps.date.day();
  hour = gps.time.hour();
  minute = gps.time.minute();
  second = gps.time.second();
  latitude = gps.location.lat();
  longitude = gps.location.lng();
  speed = gps.speed.kmph();
}

void writeInfo()
{
  float falt, h_dop1;
  setData();
  falt = gps.altitude.meters();
  h_dop1 = gps.hdop.hdop();
  h_dop1 = h_dop1 / 100;
  hour = hour + 1; //set your time zone, my zone is +1
  if (hour > 23)
  {
    hour = hour - 24;
    day += 1;
  }
  year -= 2000;
  sprintf(date1, "%4d-%02d-%02dT%02d:%02d:%02dZ", year, month, day, hour, minute, second);
  Serial.println(date1);
  unsigned long start = millis();
  if (start > 10000) //delay for start-
  {
    dataFile = SD.open(filepath, (O_READ | O_WRITE));
    unsigned long filesize = dataFile.size();
    // back up the file pointer to just before the closing tags
    filesize -= 27;
    dataFile.seek(filesize);
    dataFile.print(F("<trkpt lat=\""));
    dataFile.print(latitude, 7);
    dataFile.print(F("\" lon=\""));
    dataFile.print(longitude, 7);
    dataFile.println(F("\">"));
    dataFile.print(F("<time>"));
    dataFile.print(date1);
    dataFile.println(F("</time>"));
    dataFile.print(F("<ele>"));
    dataFile.print(falt, 1);
    dataFile.print(F("</ele>\r\n<hdop>"));
    //dataFile.print(F("<hdop>"));
    dataFile.print(h_dop1, 3);
    dataFile.println(F("</hdop>\r\n</trkpt>"));
    dataFile.print(F("</trkseg>\r\n</trk>\r\n</gpx>\r\n"));
    dataFile.close();
    //Serial.println("Wrote info");
    //Serial.print(F("Satellites: "));
    //printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
    gps.satellites.value();
    Serial.println();
  }
  smartDelay(1000); //writing gps point every 1 sec
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

/*static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}*/

void volti(){
  // read the input on analog pin 0:
  int sensorValue = analogRead(A7);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (3.7 / 765.0);
  // print out the value you read:
  Serial.println(voltage);
}
