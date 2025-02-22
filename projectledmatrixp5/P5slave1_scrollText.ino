/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Modified from the examples of the Arduino LoRa library
  More resources: https://RandomNerdTutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/
*********/

#include <SPI.h>
#include <LoRa.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// define chan cam
#define R1_PIN 32
#define G1_PIN 21
#define B1_PIN 33
#define R2_PIN 25
#define G2_PIN 17
#define B2_PIN 26

#define A_PIN 27
#define B_PIN 16
#define C_PIN 12
#define D_PIN 4
#define E_PIN -1

#define LAT_PIN 15
#define OE_PIN 22
#define CLK_PIN 14

#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 1

//define the pins used by the transceiver module
#define ss 5
#define rst 13
#define dio0 2
int counter = 0;
// 
MatrixPanel_I2S_DMA *dma_display = nullptr;
uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Start");
  delay(1000);
  ///////////////////////////////
  HUB75_I2S_CFG::i2s_pins _pins = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
  delay(10);

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, _pins);
  delay(10);

  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
  delay(10);

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(35);
  
  dma_display->clearScreen();
  
  dma_display->fillScreen(myWHITE);
  delay(1000);
  dma_display->fillScreen(myRED);
  delay(1000);
  dma_display->fillScreen(myGREEN);
  delay(1000);
  dma_display->fillScreen(myBLUE);
  delay(1000);
  
  dma_display->clearScreen();
  delay(1000);
  //////////////
 
  /////////////////
  while (!Serial);
  Serial.println("LoRa Receiver");
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //868E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  ///////////////////
}

// cuon text


void loop() {
  
  // try to parse packet
  String LoRaData = "";
  dma_display->setTextSize(1);    
  dma_display->setTextWrap(false);
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    
    while (LoRa.available()) {
     
      LoRaData += LoRa.readString();
      Serial.print(LoRaData); 
      // Scrolling_text(60, LoRaData, myRED);
      dma_display->fillScreen(myBLACK); // Xóa màn hình
      dma_display->setCursor(0, 12); // Đặt vị trí con trỏ
      dma_display->setTextColor(myRED); // Đặt màu văn bản
      dma_display->print(LoRaData); // In văn bản
    }
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

     // send feedback
    LoRa.beginPacket();
    LoRa.print("da nhan du lieu");
    LoRa.print(counter);
    LoRa.endPacket();
  }
  
}
