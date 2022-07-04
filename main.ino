#include <rgb_lcd.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ElegantOTA.h>
// Variables
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const int MaxHeight = 1700;  //height of water tank in mm
const int Offset = 100;      //distance between sensor and top of water tank in mm
const int MaxVol = 20000;    //max vol of both water tanks in L
int CurVol = 0;
int CurPercentage = 0;
// Sensor variables
int pinRX = 16;
int pinTX = 17;
unsigned char data_buffer[4] = { 0 };
float distance = 0;
unsigned char CS;
// Object to represent software serial port A02YYUW
SoftwareSerial MySerial(pinRX, pinTX);
// Object to represent 16x2 LCD
rgb_lcd lcd;
// Webserver and OTA server
WiFiServer webserver(80);  //Json server
WebServer server(81);      //OTA server

// Progress Bar characters
byte zero[] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000 };
byte one[] = { B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000 };
byte two[] = { B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000 };
byte three[] = { B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100 };
byte four[] = { B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110 };
byte five[] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };

// Function to start Wifi server
void setupWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("WiFi connected."));
  Serial.print(F("Please connect to http://"));
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.print(WiFi.localIP());
  // Start JSON webserver
  webserver.begin();
  Serial.println("Webserver started (:80)");
  // Start ElegantOTA
  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("OTA server started (:81)");
}

// Function to setup LCD screen
void setupLcd() {
  lcd.begin(16, 2);
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.clear();
}

// Function to update LCD screen with readings
void updateLCD() {
  static unsigned long timer = 0;
  const unsigned long interval = 1000UL * 5;  // testing, show every x seconds
  const int lineToPrintOn = 1;
  if (millis() - timer >= interval) {
    timer = millis();
    MySerial.flush();
    if (MySerial.available() > 0) {
      delay(5);  // to allow signal to stabilize
      if (MySerial.read() == 0xff) {
        data_buffer[0] = 0xff;
        for (int i = 1; i < 4; i++) {
          data_buffer[i] = MySerial.read();
        }
        CS = data_buffer[0] + data_buffer[1] + data_buffer[2];
        if (data_buffer[3] == CS) {
          distance = (data_buffer[1] << 8) + data_buffer[2];
          // Print to serial monitor
          Serial.print("Distance : ");
          Serial.print(distance);
          Serial.println(" mm");
        }
      }
      // Calculate percentage & volume
      CurPercentage = (100 - (((distance - Offset) / MaxHeight) * 100));
      CurVol = MaxVol / 100 * CurPercentage;
      // Show progress bar on LCD
      float factor = 100.0 / 80.0;  // 16 characters x 5
      int percent = (CurPercentage + 1) / factor;
      int number = percent / 5;
      int remainder = percent % 5;
      // show current volume and percentage on LCD
      if (number > 0) {
        for (int j = 0; j < number; j++) {
          lcd.setCursor(j, lineToPrintOn);
          lcd.write(5);
        }
      }
      lcd.setCursor(number, 1);
      lcd.write(remainder);
      if (number < 16) {
        for (int j = number + 1; j <= 16; j++) {
          lcd.setCursor(j, lineToPrintOn);
          lcd.write((byte)0x00);
        }
      }
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(1, 0);
      lcd.print(CurVol);
      lcd.print(" L - ");
      lcd.print(CurPercentage);
      lcd.print(" %");
    }
  }
}

void doWiFi() {
  WiFiClient client = webserver.available();
  if (!client)
    return;
  Serial.println(F("New client"));
  while (client.available()) client.read();
  StaticJsonDocument<100> doc;
  doc["data"] = CurPercentage;
  doc["distance"] = distance;

  // Write response headers
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();
  serializeJsonPretty(doc, client);
  delay(5000); //wait for client to download information
  client.stop();
}

// ========== MAIN FUNCTIONS: SETUP & LOOP ==========
void setup() {
  Serial.begin(115200);
  MySerial.begin(9600);  //Software serial port
  setupLcd();
  lcd.print("Starting...");
  delay(5000);
  setupWiFi();
  delay(5000);
  lcd.clear();
}

void loop() {
  updateLCD();
  doWiFi();
  server.handleClient();  //OTA server
}
