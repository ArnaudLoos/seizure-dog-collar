#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
//#include <InfluxData.h>
#include <InfluxDb.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#define INFLUXDB_HOST "<InfluxDB-IP>"
#define WIFI_SSID "<WiFi-SSID>"
#define WIFI_PASS "<WiFi-Password>"

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

ESP8266WiFiMulti WiFiMulti;
Influxdb influx(INFLUXDB_HOST);

void setup() {
  //Serial.begin(9600);
  //Serial.println(" ### Hello ###");

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  //Serial.print("Connecting to WIFI");
  //while (WiFiMulti.run() != WL_CONNECTED) {
    //Serial.print(".");
    //delay(100);
  //}
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());

  lis.begin(0x18);
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  //Serial.println("LIS3DH found!");

  // Set the sensitivity to 2, 4, 8 or 16 G!
  //https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout/arduino#accelerometer-ranges-4-21
  lis.setRange(LIS3DH_RANGE_4_G);   

  influx.setDb("accel");  //The name of the DB on the Influx server

  //Serial.println("Setup done");
}


//int loopCount = 0;

void loop() {
  //loopCount++;

  lis.read();      // get X Y and Z data at once

  //Serial.print("X:  "); Serial.print(lis.x); 
  //Serial.print("  \tY:  "); Serial.print(lis.y); 
  //Serial.print("  \tZ:  "); Serial.print(lis.z); 

  sensors_event_t event; 
  lis.getEvent(&event);
  
  /* Display the results (acceleration is measured in m/s^2) */
  //Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  //Serial.print(" \tY: "); Serial.print(event.acceleration.y); 
  //Serial.print(" \tZ: "); Serial.print(event.acceleration.z); 
  //Serial.println(" m/s^2 ");

  //Serial.println();
  
  InfluxData row("position");
  //row.addTag("device", "alpha");
  //row.addValue("loopCount", loopCount);
  
  //I'm grabbing both the raw Accelerometer data as well as the Normalized data.
  //https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout/arduino#raw-data-readings-4-24
  row.addValue("coord_x", lis.x);
  row.addValue("coord_y", lis.y);
  row.addValue("coord_z", lis.z);
  row.addValue("accel_x", event.acceleration.x);
  row.addValue("accel_y", event.acceleration.y);
  row.addValue("accel_z", event.acceleration.z);

  //Submit the data to InfluxDB
  influx.write(row);

  delay(200);
}
