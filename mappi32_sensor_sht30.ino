//Selamat Berkarya
//KMTek

#include <WiFi.h>
#include <SPI.h>
#include "ThingSpeak.h"
#include "Wire.h"
#include "SHT31.h"

String apiKey = "TW03WPISZEOOH2PX";

const char* ssid = "Rebstud";
const char* password = "tokoyangbaik";
const char* server = "api.thingspeak.com";

SHT31 sht;
WiFiClient  client;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  sht.begin(0x44);    //Sensor I2C Address

  Wire.setClock(100000);
  uint16_t stat = sht.readStatus();
  Serial.print(stat, HEX);
  Serial.println();

  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  sht.read();

  float temperature = sht.getTemperature();
  float humidity = sht.getHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature);
    postStr += "&field2=";
    postStr += String(humidity);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature:");
    Serial.print(sht.getTemperature(), 1);
    Serial.print("\t");
    Serial.print("Humidity:");
    Serial.println(sht.getHumidity(), 1);
    delay(1000 );
  }
  client.stop();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}
