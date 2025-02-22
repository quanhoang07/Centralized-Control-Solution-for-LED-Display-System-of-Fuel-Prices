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

#define ADDRESS_MASTER 0x01
#define ADDRESS_SLAVE1 0x02
#define ADDRESS_SLAVE2 0x03


int counter = 0;
unsigned long time2 = 0;
// 
MatrixPanel_I2S_DMA *dma_display = nullptr;
uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);


// send respond
void sendResponseToMaster() {
    LoRa.beginPacket();
    LoRa.write(ADDRESS_MASTER); // Ghi địa chỉ của master
    // LoRa.print(response); // Ghi phản hồi vào gói
    LoRa.print("da nhan du lieu");
    LoRa.endPacket();
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Start");
  delay(500);
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
  delay(500);
  dma_display->fillScreen(myRED);
  delay(500);
  dma_display->fillScreen(myGREEN);
  delay(500);
  dma_display->fillScreen(myBLUE);
  delay(500);
  
  dma_display->clearScreen();
  delay(500);
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
  // LoRa.setSpreadingFactor(7);
  // LoRa.setSignalBandwidth(125E3);
  // LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  ///////////////////
}

// cuon text
void scrollText(String text, uint16_t color) {
  int16_t x = dma_display->width();
  int16_t y = (dma_display->height() - 8) / 2; // Center the text vertically

  while (x + text.length() * 6 > 0) { // 6 is the approximate width of each character
    dma_display->fillScreen(myBLACK); // Clear the screen
    dma_display->setCursor(x, y);
    dma_display->setTextColor(color);
    dma_display->print(text);
    dma_display->flipDMABuffer(); // Update the display
    x -= 1; // Move text to the left
    delay(50); // Adjust the speed of scrolling
  }
}

String currentMessage = ""; // Tin nhắn hiện tại để hiển thị
unsigned long lastScrollTime = 0; // Thời gian lần cuối cập nhật cuộn
int16_t scrollX = 0; // Vị trí x hiện tại của tin nhắn
bool isScrolling = false; // Trạng thái cuộn tin nhắn

void updateDisplay() {
  if (isScrolling && currentMessage != "") {
    unsigned long currentTime = millis();
    if (currentTime - lastScrollTime > 50) { // Điều chỉnh tốc độ cuộn (50ms)
      lastScrollTime = currentTime;
      
      // Xóa màn hình và cập nhật vị trí tin nhắn
      dma_display->fillScreen(myBLACK);
      dma_display->setCursor(scrollX, (dma_display->height() - 8) / 2);
      dma_display->setTextColor(myRED);
      dma_display->print(currentMessage);
      dma_display->flipDMABuffer();
      
      // Di chuyển text sang trái
      scrollX -= 1;
      if (scrollX + currentMessage.length() * 6 < 0) {
        scrollX = dma_display->width(); // Reset lại vị trí để cuộn từ đầu
      }
    }
  }
}

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
      uint8_t senderAddress = LoRa.read(); // Đọc địa chỉ người gửi
      LoRaData += LoRa.readString(); // Đọc dữ liệu
      if (senderAddress == ADDRESS_SLAVE1) {
        Serial.print(LoRaData); 
        // scrollText(LoRaData, myRED);
        // dma_display->fillScreen(myBLACK); // Xóa màn hình
        // dma_display->setCursor(0, 12); // Đặt vị trí con trỏ
        // dma_display->setTextColor(myRED); // Đặt màu văn bản
        // dma_display->print(LoRaData); // In văn bản
        currentMessage = LoRaData;
        scrollX = dma_display->width()
        isScrolling = true; // Kích hoạt trạng thái cuộn
      }
      // print RSSI of packet
      Serial.print("' with RSSI ");
      Serial.println(LoRa.packetRssi());
    }
     // send feedback
    for (int i=0;i<5;){
      if ( (unsigned long) (millis() - time2) > 2000 ) {
          Serial.print("Sending packet: ");
          Serial.println(counter);
          sendResponseToMaster();
          counter++;
          time2 = millis();
          i++;
        }
     }
  }
  updateDisplay();
  
}