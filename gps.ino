#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>


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
float latitude, longitude, speed, altitude, h_dop;
byte month, day, hour, minute, second;
int year;
File dataFile;

Adafruit_ST7735 tft = Adafruit_ST7735(10, 8, 9);
SoftwareSerial ss(3, 2);

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  pinMode(6,OUTPUT);
  digitalWrite(6, HIGH);
  tft.initR(INITR_BLACKTAB); //Init
  tft.fillScreen(ST7735_BLACK); //Fill with black
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK); //White text, black background
  tft.setRotation(1);
  tft.setTextSize(1);  
  smartDelay(1000);

  while (true)
  {
    if (!(SD.begin(4))){ 
    }
    else{
      break;
    }
  }

  while (true)
  {
    
    unsigned long start = millis();
    if (gps.satellites.value())
    {
      hour = hour + 2;
      if (hour > 23)
      {
        hour = hour - 24;
        day += 1;
      }
      setData();
      sprintf(filename1, "/%02d-%02d-%02d", day, month, year-2000);
      sprintf(filepath, "/%02d-%02d-%02d/%02d-%02d%s",day, month, year-2000,  hour, minute, ".GPX");
      Serial.println(filename1);
      Serial.println(filepath);
      SD.mkdir(filename1);
      tft.setCursor(0,0);
      tft.println("St");
      tft.setCursor(50,0);
      tft.println(hour);
      tft.setCursor(60,0);
      tft.println(":");
      tft.setCursor(65,0);
      tft.println(minute);
      //volti();

      if (!SD.exists(filepath))
      {
        dataFile = SD.open(filepath, FILE_WRITE);
        dataFile.print(
              "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
              "<gpx version=\"1.1\" creator=\"Batuev\" xmlns=\"http://www.topografix.com/GPX/1/1\" \r\n"
              "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n"
              "xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\r\n"
              "\t<trk>\r\n<trkseg>\r\n");  //heading of gpx file 
        dataFile.print(F("</trkseg>\r\n</trk>\r\n</gpx>\r\n"));
        dataFile.close();
      }
    }
    else {
      tft.setCursor(0,8);
      tft.println("Seek");
      printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
    }
    break;
  }
}
void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      writeInfo();
      tft.setCursor(0,16);
      tft.println("Spd");
      tft.setCursor(40,16);
      tft.println(speed);
      tft.setCursor(0,24);
      tft.println("Alt");
      tft.setCursor(40,24);
      tft.println(altitude);
      tft.setCursor(0,32);
      tft.println("Cur");
      tft.setCursor(40,32);
      tft.println(hour);
      tft.setCursor(50,32);
      tft.println(":");
      tft.setCursor(55,32);
      tft.println(minute);
    }
  }
}

static void setData()
{
  month = gps.date.month();
  day = gps.date.day();
  hour = gps.time.hour();
  minute = gps.time.minute();
  second = gps.time.second();
  year = gps.date.year();
  latitude = gps.location.lat();
  longitude = gps.location.lng();
  speed = gps.speed.kmph();
  altitude = gps.altitude.meters();
  h_dop = gps.hdop.hdop() / 100;
  //volti();
}

void writeInfo()
{
  setData();
  start = millis();
   hour = hour + 2;
       if (hour > 23) {hour = hour - 24; day +=1;}
       year -= 2000;
    sprintf(date1, "%4d-%02d-%02dT%02d:%02d:%02dZ",year, month, day, hour,minute,second);
     Serial.println(date1);
  if ( start > 2000) //delay for start
  {
    dataFile = SD.open(filepath, FILE_WRITE);
    unsigned long filesize = dataFile.size();
     // back up the file pointer to just before the closing tags
    filesize -= 27;
    dataFile.seek(filesize);
    dataFile.print(F("<trkpt lat=\"")); 
    dataFile.print(latitude,7);
    dataFile.print(F("\" lon=\""));
    dataFile.print(longitude,7);
    dataFile.println(F("\">"));
    dataFile.print(F("<time>"));
    dataFile.print(date1);
    dataFile.println(F("</time>"));      
    dataFile.print(F("<ele>")); 
    dataFile.print(altitude,1);
    dataFile.print(F("</ele>\r\n<hdop>")); 
    //dataFile.print(F("<hdop>")); 
    dataFile.print(h_dop,3);
    dataFile.println(F("</hdop>\r\n</trkpt>"));
    dataFile.print(F("</trkseg>\r\n</trk>\r\n</gpx>\r\n"));
    dataFile.close();
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

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  //Serial.println(sz);
  smartDelay(0);
}

void volti() {
  int val = analogRead(A7);
  float voltage = val * (3.7 / 1023.0);
  tft.setCursor(0,8);
  tft.println("volt:");
  tft.setCursor(25,8);
  tft.println(voltage);
}
