//reference sites:
//https://cplusplus.com/reference/ctime/strftime/
//https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
//#include <WiFi.h>
#include "time.h"
#include<WiFiManager.h>



#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define triggerButton 0
#define DHTPIN 17
#define DHTTYPE DHT11   // DHT 11

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//const char* ssid       = "PiServe_Corp";
//const char* password   = "tech121login*";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

char timeHour[3];
char timeMinute[4];
char dayName[10];
char weekDay[10];
char monthName[10];
unsigned long  previousMillis1, previousMillis2, previousMillis3;
DHT dht(DHTPIN, DHTTYPE);
float t, h;
WiFiManager wm;

void setup()
{
  pinMode(triggerButton, INPUT);
  dht.begin();
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print("Hey!");
  display.setCursor(0, 30);
  display.setTextSize(2);
  display.print("Connect me");
  display.display();

  //connect to WiFi
  //  Serial.printf("Connecting to %s ", ssid);
  //  WiFi.begin(ssid, password);
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }
  //  Serial.println(" CONNECTED");
  //reset settings - for testing

  bool res;
  res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
  }
  else
  {
    Serial.println("Connected");
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.print("Connected");
    display.display();
    delay(1000);
  }
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);




}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  strftime(timeHour, 3, "%H", &timeinfo);
  //Serial.print(timeHour);
  strftime(timeMinute, 4, "%M", &timeinfo);
  //Serial.print(timeMinute);
  //Serial.println(" ");
  strftime(monthName, 5, "%b", &timeinfo);
  //Serial.print(monthName);
  //Serial.println(" ");
  strftime(dayName, 5, "%a,", &timeinfo);
  //Serial.println(dayName);
  strftime(weekDay, 3, "%d", &timeinfo);
  //Serial.println(weekDay);

}
void twoDotblink()
{

  unsigned long currentMillis1 = millis();
  if ((currentMillis1 - previousMillis1) >= 1000)
  {
    previousMillis1 = currentMillis1;
    display.setCursor(52, 3);
    display.setTextSize(5);
    display.print(":");
    display.display();


  }
  unsigned long currentMillis2 = millis();
  if ((currentMillis2 - previousMillis2) >= 2000)
  {
    previousMillis2 = currentMillis2;
    display.setCursor(52, 3);
    display.setTextSize(5);
    display.print(" ");
    display.display();


  }


}


void oled_display()
{
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setCursor(0, 5);
  display.setTextSize(4);
  display.print(timeHour);

  display.setCursor(80, 5);
  display.setTextSize(4);
  display.print(timeMinute);


  display.setCursor(0, 50);
  display.setTextSize(2);
  display.print(dayName);
  display.setCursor(50, 50);
  display.setTextSize(2);
  display.print(weekDay);
  display.setCursor(80, 50);
  display.setTextSize(2);
  display.print(monthName);
  display.display();

  unsigned long currentMillis3 = millis();
  if (currentMillis3 - previousMillis3 >= 8000)
  {
    previousMillis3 = currentMillis3;
    display.fillRect(0, 50, 150, 20, BLACK);
    display.display();

    display.setTextColor(WHITE);
    display.setCursor(0, 50);
    display.setTextSize(2);
    display.print(t, 1);
    display.setCursor(50, 50);
    display.print(char(247));
    display.print("C");
    display.setTextColor(WHITE);
    display.setCursor(90, 50);
    display.setTextSize(2);
    display.print(h, 0);
    display.print("%");
    display.display();
  }
}
void tempHumidmeasure()
{
  delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.println(t);
  Serial.println(h);
}

void buttonSense()
{
  if (digitalRead(triggerButton) == LOW)
  {
    wm.resetSettings();
    ESP.restart();
  }
}

void loop()
{
  buttonSense();
  tempHumidmeasure();
  printLocalTime();
  oled_display();
  twoDotblink();
}
