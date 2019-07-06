#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUDP.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#define WIFI_SSID "<SSID_here>"
#define WIFI_PASS "<Password>"

// the IP address of your InfluxDB host
byte host[] = {192, 168, 1, 1};

// the port that the InfluxDB UDP plugin is listening on
int port = 8089;

WiFiUDP udp;

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(9600);
  Serial.println(" ### Hello ###");

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WIFI");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lis.begin(0x18);
  //if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    //Serial.println("Couldnt start");
    //while (1);
  //}
  //Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  //Serial.println("Setup done");
}

void loop() {
  String line, x_val, y_val, z_val;

  lis.read();      // get X Y and Z data at once

  //Serial.print("X:  "); Serial.print(lis.x); 
  //Serial.print("  \tY:  "); Serial.print(lis.y); 
  //Serial.print("  \tZ:  "); Serial.print(lis.z); 

  x_val = String(abs(lis.x));
  y_val = String(abs(lis.y));
  z_val = String(abs(lis.z));
  
  // set to line protocol
  // the first value "coordinates" is the table name.
  line = String("coordinates x_coord=" + x_val + ",y_coord=" + y_val + ",z_coord=" + z_val);
  Serial.println(line);
  
  // send the packet
  udp.beginPacket(host, port);
  udp.print(line);
  udp.endPacket();

  delay(200);
}
