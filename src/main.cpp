#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>


// change next line to use with another board/shield
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "time.h"

/****
  This is an example for our Monochrome OLEDs based on SSD1306 drivers. Pick one up today in the adafruit shop! ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 pixel display using I2C to communicate 3 pins are required to interface (two I2C and one reset).
  Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. BSD license, check license.txt for more information All text above, and the splash screen below must be included in any redistribution. 
*********/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#include "secret.h"
// Replace with your network credentials in secret.h
// It just has two lines like this:
// const char *ssid     = "";
// const char *password = "";


#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2


// NTP SETUP
WiFiUDP ntpUDP;

char timestr[24];

void writeText(const char* msg) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();      // Show initial text
  delay(100);
}
void writeTextBig(const char* msg) {
  display.clearDisplay();
  display.setTextSize(4); // Draw 4X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();      // Show initial text
  delay(100);
}


void setup() {

  Serial.begin(115200);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  display.dim(true);

  writeText("Connecting...");
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  writeText("Connected");

  // Setup NTP and timezone
  configTime(0, 0,"192.168.5.1", "pool.ntp.org", "time.nist.gov");

	auto tz = "PST8PDT,M3.2.0,M11.1.0";
  setenv("TZ", tz, 1);
  tzset();
}

struct tm timeinfo;
void loop() { 
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  // Figure out AM/PM
  int hours = timeinfo.tm_hour;
  auto end = hours <= 12 ? "A" : "P";
  
  // Shift for 12 hour clock
  hours = hours <= 12 ? hours : hours - 12;
  
  snprintf(timestr, sizeof(timestr), "%d:%02d", hours, timeinfo.tm_min);
  
  // Write the time large with AM/PM smaller at the end
  display.clearDisplay();
  display.setTextSize(4); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(timestr);
  display.setTextSize(1);
  display.print(end);
  display.display();      // Show initial text
  
  delay(30 * 1000);
}
