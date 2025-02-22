#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define ss 15
#define rst 16
#define dio0 2
int counter = 0;


void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
}

void loop() 
{
                
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(3000);
}


// /////////////////////////////////////////////////////////////
// phần này là dùng cho truyền từ trên node-red xuống rồi gửi cho slave
/*
#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char *ssid = "laohac91";
const char *password = "998899889988";
const char *mqtt_server = "172.31.99.134";
const int mqtt_port = 1883;
const char *mqtt_id = "esp8266";

#define ss 15
#define rst 16
#define dio0 2
int counter = 0;

String message;

WiFiClient client;
PubSubClient mqtt_client(client);

// hàm callback server
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Recived data from: ");
  Serial.println(topic);
  Serial.print("Message: ");
  message = "";
  for (size_t i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println();
  Serial.println("------------------------------------------");
}

void setup() 
{
  Serial.begin(115200);
  // setup wifi//////////////////////////
  Serial.print("Connecting to Wifi... ");
  Serial.print(ssid);
  Serial.println();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to Wifi ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  // delay(1000);

// setup mqtt//////////////////
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);

  Serial.println("Connecting to mqtt... ");
  while (!mqtt_client.connect(mqtt_id))
  {
    delay(500);
  }
  Serial.println("Connected to mqtt ");
  mqtt_client.subscribe("to-esp8266");
  mqtt_client.publish("from-esp8266", "Hello Server");

// setup Lora////////////////////////
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
}


void loop() 
{
  mqtt_client.loop();              
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print(message + " ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(3000);
