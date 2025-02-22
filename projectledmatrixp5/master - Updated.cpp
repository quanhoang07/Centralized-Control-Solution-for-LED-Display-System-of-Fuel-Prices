/*
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
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() 
{
                
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("xin chao");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(10000);
}

*/
// /////////////////////////////////////////////////////////////
// phần này là dùng cho truyền từ trên node-red xuống rồi gửi cho slave

#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>


#define ss 15
#define rst 16
#define dio0 2

#define ADDRESS_MASTER 0x01
#define ADDRESS_SLAVE1 0x02
#define ADDRESS_SLAVE2 0x03

int counter = 0;
unsigned long time1 = 0;
unsigned long time2 = 0;
String message;

// const char *ssid = "Viet Duc 2.4G";
// const char *password = "0968199783";
// const char *mqtt_server = "192.168.1.165";

const char *ssid = "laohac91";
const char *password = "998899889988";
const char *mqtt_server = "172.31.99.31";

const int mqtt_port = 1883;
const char *mqtt_id = "esp8266";
const char *mqtt_username = "quan";  // Thay bằng username của bạn
const char *mqtt_password = "quan2002";

WiFiClient client;
PubSubClient mqtt_client(client);

// Biến toàn cục để kiểm soát vòng lặp
bool messageReceived = false;
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
  // Đặt cờ để cho biết đã nhận được tin nhắn
  messageReceived = true; 
}



void setup() 
{
  Serial.begin(115200);
  // setup wifi //////////////////////////
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

  // setup mqtt //////////////////
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);
  Serial.println("Connecting to mqtt... ");
  while (!mqtt_client.connect(mqtt_id, mqtt_username, mqtt_password))
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
  // LoRa.setSpreadingFactor(7);
  // LoRa.setSignalBandwidth(125E3);
  // LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}


void loop() 
{
  mqtt_client.loop();    
  int packetSize = LoRa.parsePacket();
  if (!packetSize) {
    if ( (unsigned long) (millis() - time1) > 5000 ) {
      Serial.print("Sending packet: ");
      Serial.println(counter);
      // send packet
      handleJsonData(message);
      counter++;
      time1 = millis();
      if (counter > 60) {
        mqtt_client.publish("from-esp8266", "End");
      }
    }
  } else {
      // Đặt lại cờ sau khi xử lý xong
      uint8_t recipientAddress = LoRa.read(); // Đọc địa chỉ người nhận
      uint8_t senderAddress = LoRa.read(); // Đọc địa chỉ người gửi
      String LoRaData = LoRa.readString(); // Đọc dữ liệu
      if (recipientAddress == ADDRESS_MASTER) {
        messageReceived = false; 
        if (senderAddress == ADDRESS_SLAVE1) {
          Serial.print("Received packet from slave 1: ");
          Serial.println(LoRaData);
          mqtt_client.publish("from-esp8266", LoRaData.c_str());
        } else if (senderAddress == ADDRESS_SLAVE2) {
          Serial.print("Received packet from slave 2: ");
          Serial.println(LoRaData);
          mqtt_client.publish("from-esp8266", LoRaData.c_str());
        } else {}
        do
        {
          mqtt_client.loop();
          if ( (unsigned long) (millis() - time2) > 10000 ) {
            Serial.println("Waittt!");
            time2 = millis();
          }
        } while (!messageReceived);
      }
  }

}

// send data
void sendDataToSlave(uint8_t slaveAddress, String data) {
    LoRa.beginPacket();
    LoRa.write(slaveAddress); // Ghi địa chỉ đích vào gói
    LoRa.print(data); // Ghi dữ liệu vào gói
    LoRa.endPacket();
}

// Hàm phân tích chuỗi json
void handleJsonData(String payload) {
  // Tạo đối tượng DynamicJsonDocument
  StaticJsonDocument<1024> doc;
  // Phân tích chuỗi JSON
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }
  String combinedString = "";
  String type ="";
  String data ="";
  // Truy cập dữ liệu từ JSON
  if (!doc.is<JsonArray>()) {
    Serial.println("JSON is not an array!");
    type = doc["type"].as<String>(); // Lấy giá trị của loại led
    int station  = doc["station"];         // Lấy giá trị của id trạm 
    data = doc["data"].as<String>();        // Lấy giá trị của data
    String code = doc["code"]; 
    float price = doc["price"]; 
    combinedString += String(code) + ": " + String(price) + "  ";
    combinedString.toUpperCase();
    
  } else {
    // Truy cập dữ liệu từ JSON
    for (JsonObject item : doc.as<JsonArray>()) {
      type = item["type"].as<String>(); // Lấy giá trị của loại led
      int station  = item["station"];         // Lấy giá trị của id trạm 
      String data = item["data"].as<String>();        // Lấy giá trị của data
      String code = item["code"]; 
      float price = item["price"]; 
      combinedString += String(code) + ": " + String(price) + "  ";
      combinedString.toUpperCase();
      // In dữ liệu ra Serial Monitor
     
    }
  }
  // In dữ liệu ra Serial Monitor
  // Serial.println("Received JSON:");
  // Serial.println(payload);

  if (type != NULL && type == "infor") {
      Serial.print("data: ");
      Serial.println(data);
      sendDataToSlave(ADDRESS_SLAVE2, data);
  } else if (type != NULL && type == "price") {
    Serial.print("data: ");
    Serial.println(combinedString);
    sendDataToSlave(ADDRESS_SLAVE1, combinedString);
  } else{}
  combinedString = "";
}

